#include "ParticleSystemComp.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

EntityManager* ParticleSystemComp::mEntityManager = nullptr;

ParticleSystemComp::ParticleSystemComp(string scriptName, UINT ID)
: Component("ParticleSystemComp")
{
	//this->mPosition = initialPosition;
	this->mScriptName = scriptName;
	this->mParticleSystemID = ID;
	this->mParticleSystemIsActive = false;
	//this->mAcceleration = Vec3(0.0f, 0.0f, 0.0f);
	//this->mDirection = Vec3(0.0f, 0.0f, 0.0f);
	//this->mRandomVelocity = Vec3(0.0f, 0.0f, 0.0f);
	//this->mAgeLimit = 0.0f;
	//this->mFadeTime = 0.0f;
	//this->mEmitFrequency = 0.0f;
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
	this->mParticleSystem->SetActive(this->mParticleSystemIsActive);

	if (this->mParticleSystemIsActive)
	{
		//Call Lua functions to update values used by ParticleSystemComp::ParticleSystem*
		std::string func = "update_" + this->mScriptName;
		lua_getglobal(mEntityManager->modules->script->L, func.c_str());
		lua_pushnumber(mEntityManager->modules->script->L, dt);

		lua_pcall(mEntityManager->modules->script->L, 1, 0, 0);
	}

	//this->mParticleSystem->SetParticleAgeLimit(this->mAgeLimit);
	//this->mParticleSystem->SetParticleFadeTime(this->mFadeTime);
	//this->mParticleSystem->SetEmitFrequency(this->mEmitFrequency);
	//this->mParticleSystem->SetConstantAccel(XMFLOAT3(this->mAcceleration.X, this->mAcceleration.Y, this->mAcceleration.Z));
	//this->mParticleSystem->SetEmitPos(XMFLOAT3(this->mPosition.X, this->mPosition.Y, this->mPosition.Z));
	//this->mParticleSystem->SetEmitDir(XMFLOAT3(this->mDirection.X, this->mDirection.Y, this->mDirection.Z));
	//this->mParticleSystem->SetRandomVelocity(XMFLOAT3(this->mRandomVelocity.X, this->mRandomVelocity.Y, this->mRandomVelocity.Z));
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

//void ParticleSystemComp::pSetEmitPos(Vec3 pos)
//{
//	//this->mPosition = pos;
//	this->mParticleSystem->SetEmitPos(XMFLOAT3(pos.X, pos.Y, pos.Z));
//}
//
//void ParticleSystemComp::pSetEmitFrequency(float frequency)
//{
//	//this->mEmitFrequency = frequency;
//	this->mParticleSystem->SetEmitFrequency(frequency);
//}
//
//void ParticleSystemComp::pSetAcceleration(Vec3 acceleration)
//{
//	//this->mAcceleration = acceleration;
//	this->mParticleSystem->SetConstantAccel(XMFLOAT3(acceleration.X, acceleration.Y, acceleration.Z));
//}
//
//void ParticleSystemComp::pSetDirection(Vec3 direction)
//{
//	this->mParticleSystem->SetEmitDir(XMFLOAT3(direction.X, direction.Y, direction.Z));
//	//this->mDirection = direction;
//}
//
//void ParticleSystemComp::pSetRandomVelocity(Vec3 velocity)
//{
//	//this->mRandomVelocity = velocity;
//	this->mParticleSystem->SetRandomVelocity(XMFLOAT3(velocity.X, velocity.Y, velocity.Z));
//}
//
//void ParticleSystemComp::pSetAgeLimit(float ageLimit)
//{
//	//this->mAgeLimit = ageLimit;
//	this->mParticleSystem->SetParticleAgeLimit(ageLimit);
//}
//
//void ParticleSystemComp::pSetFadeTime(float fadeTime)
//{
//	//this->mFadeTime = fadeTime;
//	this->mParticleSystem->SetParticleFadeTime(fadeTime);
//}
//
//void ParticleSystemComp::pSetRandomVelocityActive(bool flag)
//{
//	this->mParticleSystem->SetRandomVelocityActive(flag);
//}
//
//void ParticleSystemComp::pActivate()
//{
//	this->mIsActive = true;
//}
//
//void ParticleSystemComp::pDeactivate()
//{
//	this->mIsActive = false;
//}

int ParticleSystemComp::SetEmitPos(lua_State* L)
{
	//this->mPosition = pos;
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

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetEmitPos(XMFLOAT3(pos.X + x, pos.Y + y, pos.Z + z));
		}
	}
	return 0;
}

int ParticleSystemComp::SetEmitFrequency(lua_State* L)
{
	//this->mEmitFrequency = frequency;
	//this->mParticleSystem->SetEmitFrequency(lua_State* L);
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float emitFreq = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetEmitFrequency(emitFreq);
		}
	}
	return 0;
}

int ParticleSystemComp::SetAcceleration(lua_State* L)
{
	//this->mAcceleration = acceleration;
	//this->mParticleSystem->SetConstantAccel(XMFLOAT3(acceleration.X, acceleration.Y, acceleration.Z));
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetConstantAccel(XMFLOAT3(x, y, z));
		}

	}
	return 0;
}

int ParticleSystemComp::SetDirection(lua_State* L)
{
	//this->mParticleSystem->SetEmitDir(XMFLOAT3(direction.X, direction.Y, direction.Z));
	//this->mDirection = direction;
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetEmitDir(XMFLOAT3(x, y, z));
		}
	}

	return 0;
}

int ParticleSystemComp::SetRandomVelocity(lua_State* L)
{
	//this->mRandomVelocity = velocity;
	//this->mParticleSystem->SetRandomVelocity(XMFLOAT3(velocity.X, velocity.Y, velocity.Z));
	int n = lua_gettop(L);

	if (n >= 5)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float x = (float)lua_tonumber(L, 3);
		float y = (float)lua_tonumber(L, 4);
		float z = (float)lua_tonumber(L, 5);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetRandomVelocity(XMFLOAT3(x, y, z));
		}
	}
	return 0;
}

int ParticleSystemComp::SetAgeLimit(lua_State* L)
{
	//this->mAgeLimit = ageLimit;
	//this->mParticleSystem->SetParticleAgeLimit(ageLimit);
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float ageLimit = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetParticleAgeLimit(ageLimit);
		}
	}
	return 0;
}

int ParticleSystemComp::SetFadeTime(lua_State* L)
{
	//this->mFadeTime = fadeTime;
	//this->mParticleSystem->SetParticleFadeTime(fadeTime);
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		float fadeTime = (float)lua_tonumber(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetParticleFadeTime(fadeTime);
		}
	}
	return 0;
}

int ParticleSystemComp::SetRandomVelocityActive(lua_State* L)
{
	//this->mParticleSystem->SetRandomVelocityActive(flag);
	int n = lua_gettop(L);

	if (n >= 3)
	{
		int entityID = (int)lua_tointeger(L, 1);
		int particleSystemID = (int)lua_tointeger(L, 2);
		int flag = (int)lua_toboolean(L, 3);

		Entity* entity = mEntityManager->getEntity(entityID);

		if (entity->hasComponents("ParticleSystemComp"))
		{
			entity->getComponent<ParticleSystemComp*>("ParticleSystemComp")->GetParticleSystem()->SetRandomVelocityActive(flag ? true : false);
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

		ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
		if (psc)
		{
			psc->GetParticleSystem()->SetParticleType((ParticleType)particleType);
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

		ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
		if (psc)
		{
			psc->GetParticleSystem()->SetScale(XMFLOAT2(xScale, yScale));
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

		ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
		if (psc)
		{
			psc->GetParticleSystem()->SetActive(true);
			psc->SetParticleSystemIsActive(true);
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

		ParticleSystemComp* psc = entity->getComponent<ParticleSystemComp*>("ParticleSystemComp");
		if (psc)
		{
			psc->GetParticleSystem()->SetActive(false);
			psc->SetParticleSystemIsActive(false);
		}
	}

	return 0;
}