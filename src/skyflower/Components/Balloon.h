#ifndef BALLOON_H
#define BALLOON_H

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

class Balloon : public Component {

public:

	Balloon() : Component("Balloon")
	{
		pop = false;
	};
	virtual ~Balloon() {};

	void addedToEntity() 
	{
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

	void update(float dt)
	{
		if (pop)
		{
			Vec3 scale = getOwner()->returnScale() - getOwner()->returnScale()*dt*5;
			if (scale.X < 0.1f || scale.Y < 0.1f || scale.Z < 0.1f)
				scale = Vec3(0, 0, 0);
			getOwner()->updateScale(scale);
		}
	}

	void Pop()
	{
		pop = true;
		getOwner()->updateScale(getOwner()->returnScale()*1.4f);
		Vec3 pos = getOwner()->returnPos();
		getOwner()->getModules()->sound->PlaySound("balloon_pop.wav", 1.0f, &pos.X);
	}

private:

	bool pop;

};

#endif