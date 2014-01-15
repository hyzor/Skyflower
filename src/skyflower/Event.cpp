#include "Components/Event.h"
#include "EntityManager.h"

Cistron::EntityManager* Event::entityManager = nullptr;

int Event::PlaySound(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 2)
	{
		Cistron::EntityId Id = lua_tonumber(L, 1);
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
		Cistron::EntityId Id = lua_tonumber(L, 1);

		entityManager->sendMessageToEntity("Jump", Id);
	}

	return 0;
}

int Event::ChangeLevel(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		int level = lua_tonumber(L, 1);
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

	Entity* player = entityManager->getEntity(0);
	Vec3 pos = player->returnPos();
	if (n >= 1)
	{
		EntityId Id = lua_tonumber(L, 1); 
		
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
		EntityId spawnId = lua_tonumber(L, 1);
		EntityId pointId = lua_tonumber(L, 2);

		Entity* spawnEntity = entityManager->getEntity(spawnId);
		Entity* pointEntity = entityManager->getEntity(pointId);

		spawnEntity->spawnpos = pointEntity->returnPos();
		entityManager->sendMessageToEntity("Respawn", spawnId);
	}


	return 0;
}