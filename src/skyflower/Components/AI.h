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

	AI() : Component("AI")
	{
		centerradius = 0;
		attacktime = 0;
		nextattack = 10;
		curDir = Vec3();
	};
	virtual ~AI() {};

	void addedToEntity() {
		cout << "A AI was added to the system." << endl;

		requestMessage("MoveToTarget", &AI::MoveToTarget);
		requestMessage("StopMoving", &AI::stopMoving);
		requestMessage("StartMoving", &AI::startMoving);
	}

	void sendAMessage(string message)	{
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

	void stopMoving(Message const& msg)
	{
		cout << "STOP MOVING" << endl;
		this->canMove = false;
	}

	void startMoving(Message const& msg)
	{
		this->canMove = true;
	}
};

#endif