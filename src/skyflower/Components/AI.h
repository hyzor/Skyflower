#ifndef AI_H
#define AI_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Collision.h"
#include "PotentialField.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class AI : public Component {

public:

	// constructor - age is fixed at creation time
	AI() : Component("AI")
	{
		centerradius = 0;
		attacktime = 0;
		nextattack = 10;
		curDir = Vec3();
	};
	virtual ~AI() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity();

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	void update(float dt);

private:
	Vec3 curDir;
	float centerradius;
	float attacktime;
	float nextattack;
	void Attack(Message const& msg)
	{
		centerradius = 0;
		attacktime = 2;
		nextattack = 5;
	}

};

#endif