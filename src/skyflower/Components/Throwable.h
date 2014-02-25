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
		this->targetPos = Vec3();
	}
	virtual ~Throwable() {};

	void addedToEntity()
	{
		requestMessage("beingThrown", &Throwable::beingThrown);
		requestMessage("stopBeingThrown", &Throwable::stopBeingThrown);
		requestMessage("Dropped", &Throwable::dropped);

		this->p = getOwner()->getPhysics();
		getOwner()->sphere->Radius = 3.5f; // Throwable size
	}

	void setIsBeingThrown(bool state, EntityId throwerId = -1);

	void setIsBeingPickedUp(bool state, EntityId throwerId = -1);
	bool getIsBeingPickedUp();

	PhysicsEntity*getPhysicsEntity()
	{
		return p;
	}

private:

	bool isBeingPickedUp;
	bool isBeingThrown;
	EntityId throwerId;
	PhysicsEntity* p;
	Vec3 targetPos;

	void update(float deltaTime);
	void setTargetPos(Vec3 pos);
	Vec3 getTargetPos();

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