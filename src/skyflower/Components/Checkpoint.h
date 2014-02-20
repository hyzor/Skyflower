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

	Checkpoint(Vec3 spawnpoint);
	virtual ~Checkpoint();

	void addedToEntity();
	void removeFromEntity();

	void update(float dt);

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

private:

	void Activate(Message const& msg);

	void Deactivate();

	float activated;
	Vec3 spawnpoint;
	ParticleSystem *particleSystem;
};

#endif