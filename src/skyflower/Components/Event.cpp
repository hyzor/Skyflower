#include "Components/Event.h"
#include "EntityManager.h"
#include "Components/Throw.h"

// Must be included last!
#include "shared/debug.h"

Cistron::EntityManager* Event::entityManager = nullptr;
Event* Event::self = nullptr;



// we are added to an Entity, and thus to the component system
void Event::addedToEntity() {

	requestMessage("Activated", &Event::Activated);
	requestMessage("Deactivated", &Event::Deactivated);
	//requestMessage("Goal", &Event::Goal);

	
}


void Event::update(float deltaTime)
{
	if (firstUpdate)
	{
		entityManager = getEntityManager();

		std::string func = "load_" + file;
		lua_getglobal(entityManager->modules->script->L, func.c_str());
		lua_pushinteger(entityManager->modules->script->L, this->getOwnerId());

		self = this;
		lua_pcall(entityManager->modules->script->L, 1, 0, 0);


		firstUpdate = false;
	}
	if (startUpdate)
	{
		entityManager = getEntityManager();

		std::string func = "update_" + file;
		lua_getglobal(entityManager->modules->script->L, func.c_str());
		lua_pushinteger(entityManager->modules->script->L, this->getOwnerId());
		lua_pushnumber(entityManager->modules->script->L, deltaTime);

		self = this;
		lua_pcall(entityManager->modules->script->L, 2, 0, 0);
	}
}

void Event::Activated(Message const& msg)
{
	entityManager = getEntityManager();

	std::string func = "activated_" + file;
	lua_getglobal(entityManager->modules->script->L, func.c_str());
	lua_pushinteger(entityManager->modules->script->L, this->getOwnerId());

	this->activated = true;

	self = this;
	lua_pcall(entityManager->modules->script->L, 1, 0, 0);
}

void Event::Deactivated(Message const& msg)
{
	entityManager = getEntityManager();

	std::string func = "deactivated_" + file;
	lua_getglobal(entityManager->modules->script->L, func.c_str());
	lua_pushinteger(entityManager->modules->script->L, this->getOwnerId());


	this->activated = false;

	self = this;
	lua_pcall(entityManager->modules->script->L, 1, 0, 0);
}


int Event::PlaySound(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 2)
	{
		Cistron::EntityId Id = (Cistron::EntityId)lua_tointeger(L, 1);
		std::string file = lua_tostring(L, 2);

		Entity* e = entityManager->getEntity(Id);

		Vec3 position = entityManager->getEntityPos(Id);

		if (e)
			e->getModules()->sound->PlaySound(file, 1.0f, &position.X, false);
	}

	return 0;
}

int Event::Jump(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Cistron::EntityId Id = (Cistron::EntityId)lua_tointeger(L, 1);

		entityManager->sendMessageToEntity("Jump", Id);
	}

	return 0;
}

int Event::ChangeLevel(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		int level = (int)lua_tointeger(L, 1);
		if (level != 0)
			LevelHandler::GetInstance()->levelCompleted();
		LevelHandler::GetInstance()->queue(level);
	}

	return 0;
}

int Event::Level(lua_State* L)
{
	lua_pushinteger(L, LevelHandler::GetInstance()->currentLevel());
	return 1;
}

int Event::Save(lua_State* L)
{
	int n = lua_gettop(L);

	Entity* player = entityManager->getEntity(1);
	Vec3 pos = player->returnPos();
	if (n >= 1)
	{
		EntityId Id = (Cistron::EntityId)lua_tointeger(L, 1); 
		
		Entity* e = entityManager->getEntity(Id);
		pos = entityManager->getEntityPos(Id);
	}

	player->spawnpos = pos;

	return 0;
}

int Event::Load(lua_State* L)
{
	entityManager->sendMessageToEntity("Respawn", 0);

	return 0;
}

int Event::Spawn(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId spawnId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId pointId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* spawnEntity = entityManager->getEntity(spawnId);
		Entity* pointEntity = entityManager->getEntity(pointId);

		spawnEntity->spawnpos = pointEntity->returnPos();
		entityManager->sendMessageToEntity("Respawn", spawnId);
	}

	return 0;
}

int Event::StartUpdate(lua_State* L)
{
	self->startUpdate = true;
	return 0;
}

int Event::StopUpdate(lua_State* L)
{
	self->startUpdate = false;
	return 0;
}

int Event::InRange(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, (entityAi->returnPos()-entityTarget->returnPos()).Length() <= lua_tonumber(L,3));
		return 1;
	}


	lua_pushboolean(L, false);
	return 1;
}

int Event::IsActivated(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 1);

		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityTarget->getComponent<Event*>("Event")->isActivated());
		return 1;
	}


	lua_pushboolean(L, self->isActivated());
	return 1;
}

int Event::IsActivator(lua_State* L)
{
	int n = lua_gettop(L);

	
	if (n >= 2 && lua_isnumber(L, 2)) //check for id
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
		Entity* entityTarget = entityManager->getEntity((EntityId)lua_tointeger(L, 2));

		bool ret = false;
		if (entity->ground == entityTarget)
			ret = true;
		else if (entity->wall == entityTarget)
			ret = true;
		else
		{
			Touch* t = entity->getComponent<Touch*>("Touch");
			if (t)
				if (t->activator == entityTarget)
					ret = true;
		}

		lua_pushboolean(L, ret);
		return 1;
	}
	else if (n >= 2 && lua_isstring(L, 2)) //check for component
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
		std::string comp = lua_tostring(L, 2);

		bool ret = false;
		if (entity->ground)
		{
			if (entity->ground->hasComponents(comp))
				ret = true;
		}
		else if (entity->wall)
		{
			if (entity->wall->hasComponents(comp))
				ret = true;
		}
		else
		{
			Touch* t = entity->getComponent<Touch*>("Touch");
			if (t)
			if (t->activator)
			{
				if(t->activator->hasComponents(comp))
					ret = true;
			}
		}

		lua_pushboolean(L, ret);
		return 1;
	}


	lua_pushboolean(L, self->isActivated());
	return 1;
}

int Event::IsDown(lua_State* L)
{
	int n = lua_gettop(L);
	
	bool ret = false;
	
	if (n >= 1)
	{
		Entity* entityTarget = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		if(entityTarget->hasComponents("Button"))
			ret = entityTarget->getComponent<Button*>("Button")->isDown();
	}

	lua_pushboolean(L, ret);
	return 1;
}

int Event::SetTarget(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		if (n == 2)
			entityAi->getComponent<AI*>("AI")->setTarget(entityTarget, 0);
		else
			entityAi->getComponent<AI*>("AI")->setTarget(entityTarget, (float)lua_tonumber(L, 3));
	}


	return 0;
}


int Event::IsTouching(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityAi->wall == entityTarget);
		return 1;
	}


	lua_pushboolean(L, false);
	return 1;
}

int Event::IsStanding(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityAi->ground == entityTarget);
		return 1;
	}


	lua_pushboolean(L, false);
	return 1;
}

int Event::CanPush(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityAi->getComponent<Push*>("Push")->colliding(entityTarget));
		return 1;
	}


	lua_pushboolean(L, false);
	return 1;
}

int Event::push(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);
		EntityId targetId = (Cistron::EntityId)lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		entityAi->getComponent<Push*>("Push")->push(entityTarget);
	}

	return 0;
}

int Event::pushAll(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);

		Entity* entityAi = entityManager->getEntity(aiId);

		entityAi->getComponent<Push*>("Push")->pushAll();
	}

	return 0;
}

int Event::SetSpeed(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = (Cistron::EntityId)lua_tointeger(L, 1);

		Entity* entityAi = entityManager->getEntity(aiId);

		entityAi->getComponent<Movement*>("Movement")->SetSpeed((float)lua_tonumber(L, 2));
	}

	return 0;
}

int Event::MoveToTarget(lua_State* L)
{
	int n = lua_gettop(L);

	assert(n == 1);

	EntityId entityID = (Cistron::EntityId)lua_tointeger(L, 1);
	Entity *entity = entityManager->getEntity(entityID);

	if (entity)
	{
		MoveTargetComponent *component = entity->getComponent<MoveTargetComponent *>("MoveTarget");

		if (component)
			component->moveToTarget();
	}

	return 0;
}

int Event::MoveToSpawn(lua_State* L)
{
	int n = lua_gettop(L);

	assert(n == 1);

	EntityId entityID = (Cistron::EntityId)lua_tointeger(L, 1);
	Entity *entity = entityManager->getEntity(entityID);

	if (entity)
	{
		MoveTargetComponent *component = entity->getComponent<MoveTargetComponent *>("MoveTarget");

		if (component)
			component->moveToSpawn();
	}

	return 0;
}

int Event::SetContinous(lua_State* L)
{
	int n = lua_gettop(L);

	assert(n == 2);

	EntityId entityID = (Cistron::EntityId)lua_tointeger(L, 1);
	bool continous = lua_toboolean(L, 2)? true : false;
	Entity *entity = entityManager->getEntity(entityID);

	if (entity)
	{
		MoveTargetComponent *component = entity->getComponent<MoveTargetComponent *>("MoveTarget");

		if (component)
		{
			component->setContinuous(continous);
			if(continous)
				component->moveToSpawn();
		}
	}

	return 0;
}


int Event::PickUp(lua_State* L)
{
	int n = lua_gettop(L);
	assert(n == 1);

	Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

	entityManager->sendMessageToEntity("PickUp", entity->fId);


	return 0;
}


int Event::CanPick(lua_State* L)
{
	return 0;
}

int Event::sThrow(lua_State* L)
{
	int n = lua_gettop(L);
	assert(n == 1);

	Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

	entityManager->sendMessageToEntity("Throw", entity->fId);

	return 0;
}

int Event::CanThrow(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		if (n == 1)
			lua_pushboolean(L, entity->getComponent<Throw*>("Throw")->getIsHoldingThrowable());
		else
			lua_pushboolean(L, entity->getComponent<Throw*>("Throw")->getHoldingEntityId() == lua_tointeger(L, 2));
	}

	return 1;
}

int Event::Lit(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
		float time = 0.5f;
		if (n >= 2)
			time = (float)lua_tonumber(L, 2);

		PointLightComp* plc = entity->getComponent<PointLightComp*>("PointLight");
		DirectionalLightComp* dlc = entity->getComponent<DirectionalLightComp*>("DirectionalLight");
		SpotLightComp* slc = entity->getComponent<SpotLightComp*>("SpotLight");

		if (plc)
			plc->lit(time);
		if (dlc)
			dlc->lit(time);
		if (slc)
			slc->lit(time);
	}


	return 0;
}

int Event::Unlit(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
		float time = 0.5f;
		if (n >= 2)
			time = (float)lua_tonumber(L, 2);

		PointLightComp* plc = entity->getComponent<PointLightComp*>("PointLight");
		DirectionalLightComp* dlc = entity->getComponent<DirectionalLightComp*>("DirectionalLight");
		SpotLightComp* slc = entity->getComponent<SpotLightComp*>("SpotLight");

		if (plc)
			plc->unlit(time);
		if (dlc)
			dlc->unlit(time);
		if (slc)
			slc->unlit(time);
	}


	return 0;
}


int Event::IsLit(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		PointLightComp* plc = entity->getComponent<PointLightComp*>("PointLight");
		DirectionalLightComp* dlc = entity->getComponent<DirectionalLightComp*>("DirectionalLight");
		SpotLightComp* slc = entity->getComponent<SpotLightComp*>("SpotLight");

		bool lit = false;
		if (plc)
		{
			if (plc->isLit())
				lit = true;
		}
		if (dlc)
		{
			if (dlc->isLit())
				lit = true;
		}
		if (slc)
		{
			if (slc->isLit())
				lit = true;
		}

		lua_pushboolean(L, lit);
		return 1;
	}

	lua_pushboolean(L, false);
	return 1;
}

int Event::IsUnlit(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		PointLightComp* plc = entity->getComponent<PointLightComp*>("PointLight");
		DirectionalLightComp* dlc = entity->getComponent<DirectionalLightComp*>("DirectionalLight");
		SpotLightComp* slc = entity->getComponent<SpotLightComp*>("SpotLight");

		bool lit = false;
		if (plc)
		{
			if (plc->isUnlit())
				lit = true;
		}
		if (dlc)
		{
			if (dlc->isUnlit())
				lit = true;
		}
		if (slc)
		{
			if (slc->isUnlit())
				lit = true;
		}

		lua_pushboolean(L, lit);
		return 1;
	}

	lua_pushboolean(L, false);
	return 1;
}