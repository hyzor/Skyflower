#ifndef GOAL_H
#define GOAL_H

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

class Goal : public Component {

public:

	// constructor - age is fixed at creation time
	Goal() : Component("Goal")
	{
	};
	virtual ~Goal() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		requestMessage("Ground", &Goal::Finished);
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

private:

	void Finished(Message const& msg)
	{
		sendMessageToEntity(this->getOwnerId(), "Goal");
	}
};

#endif