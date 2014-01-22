#ifndef EVENT_H
#define EVENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"
#include "ScriptHandler.h"
#include "LevelHandler.h"

using namespace std;
using namespace Cistron;

class Event : public Component {

public:

	// constructor - age is fixed at creation time
	Event(std::string file) : Component("Event")
	{
		this->file = file;

		sh = new ScriptHandler();

		lua_register(sh->L, "PlaySound", Event::PlaySound);
		lua_register(sh->L, "Jump", Event::Jump);
		lua_register(sh->L, "ChangeLevel", Event::ChangeLevel);
		lua_register(sh->L, "Level", Event::Level);
		lua_register(sh->L, "Save", Event::Save);
		lua_register(sh->L, "Load", Event::Load);
		lua_register(sh->L, "Spawn", Event::Spawn);
		lua_register(sh->L, "ToggleOscillatePosition", Event::ToggleOscillatePosition);

		sh->Run(file);

	};
	virtual ~Event() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		//cout << "A Event was added to the system." << endl;

		requestMessage("Activated", &Event::Activated);
		requestMessage("Deactivated", &Event::Deactivated);
		requestMessage("Goal", &Event::Goal);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}	

	static EntityManager* entityManager;

private:

	std::string file;

	ScriptHandler *sh;

	void Activated(Message const& msg)
	{
		lua_getglobal(sh->L, "activated");
		lua_pushinteger(sh->L, this->getOwnerId());

		entityManager = getEntityManager();
		lua_pcall(sh->L, 1, 0, 0);
	}

	void Deactivated(Message const& msg)
	{
		lua_getglobal(sh->L, "deactivated");
		lua_pushinteger(sh->L, this->getOwnerId());

		entityManager = getEntityManager();
		lua_pcall(sh->L, 1, 0, 0);
	}

	void Goal(Message const& msg)
	{
		//TO DO
		cout << "GOAL" << endl;
	}

	static int PlaySound(lua_State* L);
	static int Jump(lua_State* L);
	static int ChangeLevel(lua_State* L);
	static int Level(lua_State* L);
	static int Save(lua_State* L);
	static int Load(lua_State* L);
	static int Spawn(lua_State* L);
	static int ToggleOscillatePosition(lua_State* L);
};

#endif