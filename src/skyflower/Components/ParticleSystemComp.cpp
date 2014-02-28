#include "ParticleSystemComp.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

EntityManager* ParticleSystemComp::mEntityManager = nullptr;

ParticleSystemComp::ParticleSystemComp(string scriptName, UINT ID)
: Component("ParticleSystemComp")
{
	this->mScriptName = scriptName;
	this->mParticleSystemID = ID;
	this->mParticleSystemIsActive = false;
	this->mParticleSystemFadeTime = 0.0;
	this->mParticleSystemCurrFadeTime = 0.0;
};

void ParticleSystemComp::addedToEntity()
{
	this->mParticleSystem = getOwner()->getModules()->graphics->CreateParticleSystem();
	this->mParticleSystem->SetActive(false);
	this->mEntityManager = getEntityManager();

	this->requestMessage("StartParticleSystem", &ParticleSystemComp::ActivateParticleSystem);
	this->requestMessage("StopParticleSystem", &ParticleSystemComp::DeactivateParticleSystem);
}

void ParticleSystemComp::removeFromEntity()
{
	getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystem);
	this->mEntityManager = NULL;
	this->mParticleSystem = NULL;
	this->setActive(false);
}

void ParticleSystemComp::update(float dt)
{
	//Increment fadeout timer
	if (!(this->mParticleSystemIsActive))
	{
		this->mParticleSystemCurrFadeTime += dt;
	}

	//If active, set the particle system as active
	//If inactive but fadeout time hasn't reached it's limit yet, set emit frequency so that no new particles will be emitted
	//If inactive and fadeout time reached its limit, set particle system so inactive and reset fadeout timer
	if (this->mParticleSystemIsActive)
	{
		this->mParticleSystem->SetActive(this->mParticleSystemIsActive);
	}
	else if (!this->mParticleSystemIsActive && this->mParticleSystemCurrFadeTime < this->mParticleSystemFadeTime)
	{
		this->mParticleSystem->SetEmitFrequency(FLT_MAX);
	}
	else if (!this->mParticleSystemIsActive && this->mParticleSystemCurrFadeTime > this->mParticleSystemFadeTime)
	{
		this->mParticleSystem->SetActive(this->mParticleSystemIsActive);
		this->mParticleSystemCurrFadeTime = 0.0;
	}

	//Call update function in Luascript
	if (this->mParticleSystemIsActive)
	{
		//Call Lua functions to update values used by ParticleSystemComp::ParticleSystem*
		std::string func = "update_" + this->mScriptName;
		lua_getglobal(mEntityManager->modules->script->L, func.c_str());
		lua_pushnumber(mEntityManager->modules->script->L, dt);

		lua_pcall(mEntityManager->modules->script->L, 1, 0, 0);
	}
}

void ParticleSystemComp::ActivateParticleSystem(const Message& message)
{
	this->mParticleSystemIsActive = true;

	std::string func = "start_" + this->mScriptName;
	lua_getglobal(mEntityManager->modules->script->L, func.c_str());
	lua_pcall(mEntityManager->modules->script->L, 0, 0, 0);

}

void ParticleSystemComp::DeactivateParticleSystem(const Message& message)
{
	this->mParticleSystemIsActive = false;

	std::string func = "stop_" + this->mScriptName;
	lua_getglobal(mEntityManager->modules->script->L, func.c_str());
	lua_pcall(mEntityManager->modules->script->L, 0, 0, 0);
}

void ParticleSystemComp::_SetParticleType()
{
	std::string func = "settype_" + this->mScriptName;
	lua_getglobal(mEntityManager->modules->script->L, func.c_str());
	lua_pcall(mEntityManager->modules->script->L, 0, 0, 0);
}

int ParticleSystemComp::SetEmitPos(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);
		Vec3 pos = entity->returnPos();
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetEmitPos(XMFLOAT3(pos.X + x, pos.Y + y, pos.Z + z));
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetEmitFrequency(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float emitFreq = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetEmitFrequency(emitFreq);
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetAcceleration(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetConstantAccel(XMFLOAT3(x, y, z));
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetDirection(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetEmitDir(XMFLOAT3(x, y, z));
			}
		}
	}

	return 0;
}

int ParticleSystemComp::SetRandomVelocity(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetRandomVelocity(XMFLOAT3(x, y, z));
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetAgeLimit(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float ageLimit = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetParticleAgeLimit(ageLimit);
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetFadeTime(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float fadeTime = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetParticleFadeTime(fadeTime);
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetRandomVelocityActive(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		int flag = (int)lua_toboolean(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetRandomVelocityActive(flag ? true : false);
			}
		}
	}
	return 0;
}

int ParticleSystemComp::SetParticleType(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		int particleType = (int)lua_tointeger(L, 3);


		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetParticleType((ParticleType)particleType);
			}
		}
	}

	return 0;
}

int ParticleSystemComp::SetScale(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 4)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float xScale = (float)lua_tointeger(L, 3);
		float yScale = (float)lua_tointeger(L, 4);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetScale(XMFLOAT2(xScale, yScale));
			}
		}
	}
	return 0;
}

int ParticleSystemComp::Activate(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->GetParticleSystem()->SetActive(true);
				psc->SetParticleSystemIsActive(true);
			}
		}
	}
	return 0;
}

int ParticleSystemComp::Deactivate(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				//psc->GetParticleSystem()->SetActive(false);
				psc->SetParticleSystemIsActive(false);
			}
		}
	}

	return 0;
}

int ParticleSystemComp::SetFadeLimit(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float fadeLimit = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);
		if (entity)
		{
			ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
			if (psc)
			{
				psc->SetParticleSystemFadeLimit(fadeLimit);
			}
		}
	}

	return 0;
}