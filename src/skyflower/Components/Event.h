#ifndef EVENT_H
#define EVENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"
#include "ScriptHandler.h"
#include "Components/Push.h"
#include "LevelHandler.h"

using namespace std;
using namespace Cistron;

class Event : public Component {

public:

	// constructor - age is fixed at creation time
	Event(std::string file) : Component("Event")
	{
		this->file = file;
		startUpdate = false;
		firstUpdate = true;
		activated = false;

	};
	virtual ~Event() {};

	static void Register(ScriptHandler* sh)
	{
		lua_register(sh->L, "PlaySound", Event::PlaySound);
		lua_register(sh->L, "Jump", Event::Jump);
		lua_register(sh->L, "ChangeLevel", Event::ChangeLevel);
		lua_register(sh->L, "Level", Event::Level);
		lua_register(sh->L, "Save", Event::Save);
		lua_register(sh->L, "Load", Event::Load);
		lua_register(sh->L, "Spawn", Event::Spawn);
		lua_register(sh->L, "StopUpdate", Event::StopUpdate);
		lua_register(sh->L, "StartUpdate", Event::StartUpdate);
		lua_register(sh->L, "InRange", Event::InRange);
		lua_register(sh->L, "IsActivated", Event::IsActivated);

		//AI
		lua_register(sh->L, "SetTarget", Event::SetTarget);
		lua_register(sh->L, "IsTouching", Event::IsTouching);
		lua_register(sh->L, "IsStanding", Event::IsStanding);
		lua_register(sh->L, "CanPush", Event::CanPush);
		lua_register(sh->L, "Push", Event::push);
		lua_register(sh->L, "PushAll", Event::pushAll);
		lua_register(sh->L, "SetSpeed", Event::SetSpeed);

		// MoveTarget component
		lua_register(sh->L, "MoveToTarget", Event::MoveToTarget);
		lua_register(sh->L, "MoveToSpawn", Event::MoveToSpawn);
		lua_register(sh->L, "SetContinous", Event::SetContinous);
	};

	// we are added to an Entity, and thus to the component system
	void addedToEntity();

	void sendAMessage(string message)
	{
		//cout << "hej det �r jag som ropar p� denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	void update(float deltaTime);

	static EntityManager* entityManager;
	static Event* self;

	bool isActivated()
	{
		return activated;
	}

private:

	std::string file;


	bool startUpdate;
	bool firstUpdate;

	bool activated;

	void Activated(Message const& msg);

	void Deactivated(Message const& msg);

	/*void Goal(Message const& msg)
	{
		//TO DO
		//cout << "GOAL" << endl;
	}*/

	static int PlaySound(lua_State* L);
	static int Jump(lua_State* L);
	static int ChangeLevel(lua_State* L);
	static int Level(lua_State* L);
	static int Save(lua_State* L);
	static int Load(lua_State* L);
	static int Spawn(lua_State* L);
	static int StartUpdate(lua_State* L);
	static int StopUpdate(lua_State* L);
	static int InRange(lua_State* L);
	static int IsActivated(lua_State* L);

	static int SetTarget(lua_State* L);
	static int IsTouching(lua_State* L);
	static int IsStanding(lua_State* L);
	static int CanPush(lua_State* L);
	static int push(lua_State* L);
	static int pushAll(lua_State* L);
	static int SetSpeed(lua_State* L);

	static int MoveToTarget(lua_State* L);
	static int MoveToSpawn(lua_State* L);
	static int SetContinous(lua_State* L);
};

#endif