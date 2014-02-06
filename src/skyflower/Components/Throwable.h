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
	}
	virtual ~Throwable() {};

	void addedToEntity()
	{
		requestMessage("beingThrown", &Throwable::beingThrown);
		requestMessage("stopBeingThrown", &Throwable::stopBeingThrown);

		Vec3 temp = getEntityPos();
		this->getOwner()->sphere = new Sphere(temp.X, temp.Y, temp.Z, 5);
	}

	void setIsBeingThrown(bool state);

private:

	bool isBeingPickedUp;
	bool isBeingThrown;

	void update(float deltaTime);

	void beingThrown(Message const & msg)
	{
		this->isBeingThrown = true;
	}

	void stopBeingThrown(Message const & msg)
	{
		this->isBeingThrown = false;
	}
};

#endif