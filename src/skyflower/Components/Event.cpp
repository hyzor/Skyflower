#include "Components/Event.h"
#include "EntityManager.h"
#include "Components/Throw.h"
#include "GUI.h"

// Must be included last!
#include "shared/debug.h"

Cistron::EntityManager* Event::entityManager = nullptr;
Event* Event::self = nullptr;

void Event::addedToEntity() {

	requestMessage("Activated", &Event::Activated);
	requestMessage("Deactivated", &Event::Deactivated);
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
	int id = this->getOwnerId();
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

		entityManager->getEntity(Id)->sendMessage("Jump");
	}

	return 0;
}

int Event::ChangeLevel(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		int level = (int)lua_tointeger(L, 1);

		if (LevelHandler::GetInstance()->currentLevel() != 0)
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
	//entityManager->sendMessageToEntity("Respawn", 0);

	// Denna funktion verkar död, ingen entitet har id 0?
	assert(0);

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
		spawnEntity->sendMessage("Respawn");
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

		Push* push = entityAi->getComponent<Push*>("Push");
		if(!push || !push->isResettingSpeed())
			entityAi->getComponent<Movement*>("Movement")->SetSpeed((float)lua_tonumber(L, 2));
	}

	return 0;
}


int Event::IsDizzy(lua_State* L)
{
	bool ret = false;

	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entityAi = entityManager->getEntity((Cistron::EntityId)lua_tointeger(L, 1));
		ret = entityAi->getComponent<Movement*>("Movement")->getIsDizzy();
	}

	lua_pushboolean(L, ret);
	return 1;
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
		{
			component->moveToTarget();
		}

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
	assert(n == 2);

	Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
	Entity* target = entityManager->getEntity((EntityId)lua_tointeger(L, 2));

	Throw* throwComp = entity->getComponent<Throw*>("Throw");
	throwComp->PickUp(target);

	return 0;
}

int Event::PickUpAll(lua_State* L)
{
	int n = lua_gettop(L);
	assert(n == 1);

	Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

	Throw* throwComp = entity->getComponent<Throw*>("Throw");
	throwComp->PickUpAll();

	return 0;
}


int Event::CanPick(lua_State* L)
{
	return 0;
}

int Event::sThrow(lua_State* L)
{
	int n = lua_gettop(L);
	assert(n == 2);

	Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
	Throw* throwComp = entity->getComponent<Throw*>("Throw");
	throwComp->ThrowAt(entityManager->getEntity((EntityId)lua_tointeger(L, 2)));

	return 0;
}

int Event::CanThrow(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		Throw* throwComp = entity->getComponent<Throw*>("Throw");
		if (n == 1)
			lua_pushboolean(L, throwComp->getHeldEntity() != nullptr);
		else
			lua_pushboolean(L, throwComp->getHeldEntity() && throwComp->getHeldEntity()->fId == lua_tointeger(L, 2));
	}

	return 1;
}

int Event::IsHeld(lua_State* L)
{
	bool ret = false;

	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		Throwable* throwComp = entity->getComponent<Throwable*>("Throwable");
		ret = throwComp->getIsBeingPickedUp();
	}

	lua_pushboolean(L, ret);
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


int Event::Pop(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{ 
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		if (entity->hasComponents("Balloon"))
			entity->getComponent<Balloon*>("Balloon")->Pop();
	}
	return 0;
}
int Event::PrintText(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 3)
	{
		string text = lua_tostring(L, 1);
		int x = (int)lua_tointeger(L, 2);
		int y = (int)lua_tointeger(L, 3);
		entityManager->modules->gui->printText(text, x, y);
	}

	return 0;
}

int Event::SetMorphState(lua_State* L)
{
	// ID, x, y, z
	int n = lua_gettop(L);

	if (n >= 4)
	{
		EntityId id = (EntityId)lua_tointeger(L, 1);
		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		MorphAnimation* morph = entityManager->getEntity(id)->getComponent<MorphAnimation*>("MorphAnimation");

		if (morph)
		{
			morph->setWeights(Vec3(x, y, z));
		}
	}
	return 0;
}

int Event::StartMorph(lua_State* L)
{
	// ID, x, y, z, speed
	int n = lua_gettop(L);

	if (n >= 5)
	{
		EntityId id = (EntityId)lua_tointeger(L, 1);
		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);
		float speed = (float)lua_tonumber(L, 5);

		MorphAnimation* morph = entityManager->getEntity(id)->getComponent<MorphAnimation*>("MorphAnimation");

		if (morph)
		{
			morph->startMorphing(Vec3(x, y, z), speed);
		}
	}
	return 0;
}

int Event::CompletedLevelCount(lua_State* L)
{
	int count = levelHandler->completedCount();
	lua_pushinteger(L, count);
	return 1;
}

int Event::ButtonUp(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));

		if (entity->hasComponents("Button"))
			entity->getComponent<Button*>("Button")->Deactivate();
	}
	return 0;
}

int Event::FallingPlatform(lua_State* L)
{
	bool flag = false;
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Entity* entity = entityManager->getEntity((EntityId)lua_tointeger(L, 1));
		Entity* player = entityManager->getEntity(1);


		if (player->ground != NULL)
		{
			if (player->ground->fId == entity->fId)
			{
				flag = true;
			}
		}
	}

	lua_pushboolean(L, flag);

	return 1;
}

int Event::LevelIsCompleted(lua_State* L)
{
	bool flag = false;
	int n = lua_gettop(L);

	if (n >= 1)
	{
		int levelID = (int)lua_tointeger(L, 1);
		
		if (levelHandler->isCompleted(levelID))
		{
			flag = true;
		}
	}
	
	lua_pushboolean(L, flag);

	return 1;
}

int Event::SetActivated(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 1)
	{
		int entityToActivateID = (int)lua_tointeger(L, 1);
		int active = lua_toboolean(L, 2);

		if (active)
		{
			entityManager->activateEntity(entityToActivateID);
		}
		else
		{
			entityManager->deactivateEntity(entityToActivateID);
		}
	}

	return 0;
}

int Event::OnButton(lua_State* L)
{
	bool flag = false;
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId buttonId = (EntityId)lua_tointeger(L, 1);
		EntityId boxId = (EntityId)lua_tointeger(L, 2);


		Entity* boxEntity = entityManager->getEntity(boxId);

		if (boxEntity->ground)
		{
			if (boxEntity->ground->fId == buttonId)
			{
				flag = true;
			}
		}
	}

	lua_pushboolean(L, flag);
	return 1;
}

int Event::IsAlive(lua_State* L)
{
	bool flag = false;
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId boxId = (EntityId)lua_tointeger(L, 1);

		Entity* boxEntity = entityManager->getEntity(boxId);

		if (boxEntity->hasComponents("Health"))
			flag = boxEntity->getComponent<Health*>("Health")->isAlive();
		else
			flag = boxEntity->returnPos().Y > -100;
	}

	lua_pushboolean(L, flag);
	return 1;
}

int Event::Respawn(lua_State* L)
{
	bool flag = false;
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId boxId = (EntityId)lua_tointeger(L, 1);
		Entity* boxEntity = entityManager->getEntity(boxId);

		if (boxEntity->hasComponents("Health"))
		{
			boxEntity->sendMessage("Respawn");			
		}
	}

	lua_pushboolean(L, flag);
	return 1;
}

int Event::PlayFinishedSound(lua_State* L)
{
	int n = lua_gettop(L);
	if (n >= 1)
	{
		Cistron::EntityId Id = (Cistron::EntityId)lua_tointeger(L, 1);

		Entity* e = entityManager->getEntity(Id);

		if (e)
		{
 			e->getModules()->sound->PlaySound("positive_game_sound.wav", 0.1f);
		}
	}

	return 0;
}

int Event::SetCanMoveBox(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId Id = (EntityId)lua_tointeger(L, 1);
		Entity* Entity = entityManager->getEntity(Id);

		if (Entity->hasComponents("Box"))
		{
			Entity->sendMessage("CanBeMoved");
		}
	}
	return 0;
}

int Event::SetCanNotMoveBox(lua_State* L)
{
	int n = lua_gettop(L);

	if (n >= 1)
	{
		EntityId Id = (EntityId)lua_tointeger(L, 1);
		Entity* Entity = entityManager->getEntity(Id);

		if (Entity->hasComponents("Box"))
		{
			Entity->sendMessage("CanNotBeMoved");
		}
	}
	return 0;
}
