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