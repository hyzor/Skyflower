#ifndef EVENT_H
#define EVENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"
#include "ScriptHandler.h"

using namespace std;
using namespace Cistron;

class Event : public Component {

public:

	// constructor - age is fixed at creation time
	Event(std::string file) : Component("Event")
	{
		//this->currentOwner = nullptr;
		this->file = file;
		this->L = ScriptHandler::GetInstance()->L;

		lua_register(L, "PlaySound", Event::PlaySound);
		lua_register(L, "Jump", Event::Jump);


		ScriptHandler::GetInstance()->Run(file);
	};
	virtual ~Event() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A Event was added to the system." << endl;

		requestMessage("Activated", &Event::Activated);
		requestMessage("Deactivated", &Event::Deactivated);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	

	static EntityManager* entityManager;


private:

	std::string file;

	lua_State* L;

	void Activated(Message const& msg)
	{
		lua_getglobal(L, "activated");
		lua_pushnumber(L, (int)this->getOwnerId());

		entityManager = getEntityManager();
		lua_pcall(L, 1, 0, 0);
	}

	void Deactivated(Message const& msg)
	{
		lua_getglobal(L, "deactivated");
		lua_pushnumber(L, (int)this->getOwnerId());

		entityManager = getEntityManager();
		lua_pcall(L, 1, 0, 0);
	}


	static int PlaySound(lua_State* L);
	static int Jump(lua_State* L);
};

#endif