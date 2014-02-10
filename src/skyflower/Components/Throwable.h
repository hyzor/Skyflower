#ifndef THROWABLE_H
#define THROWABLE_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Throwable : public Component {

public:

	Throwable() : Component("Throwable")
	{
		this->isBeingPickedUp = false;
		this->isBeingThrown = false;
		this->throwerId = -1;
	}
	virtual ~Throwable() {};

	void addedToEntity()
	{
		requestMessage("beingThrown", &Throwable::beingThrown);
		requestMessage("stopBeingThrown", &Throwable::stopBeingThrown);
		requestMessage("Dropped", &Throwable::dropped);

		this->p = getOwner()->getPhysics();
	}

	void setIsBeingThrown(bool state);
	

private:

	bool isBeingPickedUp;
	bool isBeingThrown;
	EntityId throwerId;
	PhysicsEntity* p;

	void update(float deltaTime);

	void beingThrown(Message const & msg)
	{
		this->isBeingThrown = true;
	}

	void stopBeingThrown(Message const & msg)
	{
		this->isBeingThrown = false;
	}

	void dropped(Message const & msg)
	{
		this->isBeingPickedUp = false;
	}
};

#endif