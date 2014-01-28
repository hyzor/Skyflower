#include "Components/Event.h"
#include "EntityManager.h"

Cistron::EntityManager* Event::entityManager = nullptr;
Event* Event::self = nullptr;



// we are added to an Entity, and thus to the component system
void Event::addedToEntity() {
	//cout << "A Event was added to the system." << endl;

	requestMessage("Activated", &Event::Activated);
	requestMessage("Deactivated", &Event::Deactivated);
	requestMessage("Goal", &Event::Goal);

	
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
		Cistron::EntityId Id = lua_tointeger(L, 1);
		std::string file = lua_tostring(L, 2);

		Entity* e = entityManager->getEntity(Id);

		Vec3 position = entityManager->getEntityPos(Id);

		if (e)
			e->getModules()->sound->PlaySound(file, &position.X, 1.0f, false);
	}

	return 0;
}

int Event::Jump(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Cistron::EntityId Id = lua_tointeger(L, 1);

		entityManager->sendMessageToEntity("Jump", Id);
	}

	return 0;
}

int Event::ChangeLevel(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		int level = lua_tointeger(L, 1);
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
		EntityId Id = lua_tointeger(L, 1); 
		
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
		EntityId spawnId = lua_tointeger(L, 1);
		EntityId pointId = lua_tointeger(L, 2);

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
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

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
		EntityId targetId = lua_tointeger(L, 1);

		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityTarget->getComponent<Event*>("Event")->isActivated());
		return 1;
	}


	lua_pushboolean(L, self->isActivated());
	return 1;
}

int Event::SetTarget(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 2)
	{
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

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
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

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
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

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
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

		Entity* entityAi = entityManager->getEntity(aiId);
		Entity* entityTarget = entityManager->getEntity(targetId);

		lua_pushboolean(L, entityAi->getComponent<Push*>("Push")->canPush(entityTarget));
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
		EntityId aiId = lua_tointeger(L, 1);
		EntityId targetId = lua_tointeger(L, 2);

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
		EntityId aiId = lua_tointeger(L, 1);

		Entity* entityAi = entityManager->getEntity(aiId);

		entityAi->getComponent<Push*>("Push")->pushAll();
	}

	return 0;
}

int Event::MoveToTarget(lua_State* L)
{
	int n = lua_gettop(L);

	assert(n == 1);

	EntityId entityID = lua_tointeger(L, 1);
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

	EntityId entityID = lua_tointeger(L, 1);
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

	EntityId entityID = lua_tointeger(L, 1);
	bool continous = lua_toboolean(L, 2)? true : false;
	Entity *entity = entityManager->getEntity(entityID);

	if (entity)
	{
		MoveTargetComponent *component = entity->getComponent<MoveTargetComponent *>("MoveTarget");

		if (component)
			component->setContinuous(continous);
	}

	return 0;
}
