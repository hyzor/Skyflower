#ifndef CHECKPOINT_H
#define CHECKPOINT_H

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

class Checkpoint : public Component {

public:

	// constructor - age is fixed at creation time
	Checkpoint(Vec3 spawnpoint) : Component("Checkpoint")
	{
		this->spawnpoint = spawnpoint;
		activated = 0;
	};
	virtual ~Checkpoint() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A AI was added to the system." << endl;

		requestMessage("Ground", &Checkpoint::Activate);
		requestMessage("Wall", &Checkpoint::Activate);
		requestMessage("Checkpoint", &Checkpoint::Activate);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	void update(float dt);
private:

	void Activate(Message const& msg);

	void Deactivate();

	float activated;
	Vec3 spawnpoint;

};

#endif