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
		activated = 0.0f;
	};
	virtual ~Goal() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		requestMessage("Ground", &Goal::Activate);
		requestMessage("Wall", &Goal::Activate);
		requestMessage("Goal", &Goal::Activate);
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}
	void update(float dt);
private:
	void Activate(Message const& msg);

	void Deactivate();

	float activated;
};

#endif