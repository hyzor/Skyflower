#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Throw : public Component {

public:

	Throw() : Component("Throw")
	{
		this->holdingEntityId = 0;
		this->toPickUp = false;
		this->toPutDown = false;
		this->isHoldingThrowable = false;
		this->toThrow = false;
	}
	virtual ~Throw() {};

	void addedToEntity()
	{
		requestMessage("PickUpOrDown", &Throw::pickUpOrDown);
		requestMessage("Throw", &Throw::Throwing);
		requestMessage("StopThrow", &Throw::stopThrowing);
		requestMessage("DropThrowable", &Throw::dropThrowable);
	}

	//for throw and throwing
	bool getToPickUp()
	{
		return this->toPickUp;
	}

	void setToPickUp(bool state)
	{
		this->toPickUp = state;
	}

	bool getToPutDown()
	{
		return this->toPutDown;
	}

	void setToPutDown(bool state)
	{
		this->toPutDown = state;
	}

	bool getIsHoldingThrowable()
	{
		return this->isHoldingThrowable;
	}

	void setIsHoldingThrowable(bool state)
	{
		this->isHoldingThrowable = state;
	}

	bool getToThrow()
	{
		return this->toThrow;
	}

	void setToThrow(bool state)
	{
		this->toThrow = state;
	}

	void setHoldingEntityId(EntityId id)
	{
		this->holdingEntityId = id;
	}

	EntityId getHoldingEntityId()
	{
		return this->holdingEntityId;
	}

private:

	bool toPickUp;
	bool toPutDown;
	bool isHoldingThrowable;
	bool toThrow;
	EntityId holdingEntityId;

	void printToAll(Message const & msg)
	{

	}
	void update(Message const & msg)
	{
		sendMessageToEntity(this->getOwnerId(), "update");
	}

	//when pick up or put down-button is pressed
	void pickUpOrDown(Message const & msg)
	{
		//if the entity is holding something, it should try to put it down
		if (isHoldingThrowable)
		{
			setToPickUp(false);
			setToPutDown(true);
		}
		//if the entity is not holding something, it should try to pick an object
		else
		{
			setToPickUp(true);
			setToPutDown(false);
		}
	}

	//if the entity should throw something away
	void Throwing(Message const & msg)
	{
		setToThrow(true);
	}

	//if the entity should stop to try to throw something.
	void stopThrowing(Message const & msg)
	{
		setToThrow(false);
	}

	void dropThrowable(Message const & msg)
	{
		if (isHoldingThrowable)
		{
			setToPutDown(false);
			setIsHoldingThrowable(false);
			setToPickUp(false);
			sendMessageToEntity(holdingEntityId, "Dropped");
		}
	}
};

#endif