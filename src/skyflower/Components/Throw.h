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
		this->holdingEntityId = -1;
		this->toPickUp = false;
		this->toPutDown = false;
		this->isHoldingThrowable = false;
		this->toThrow = false;
		this->isDizzy = false;
	}
	virtual ~Throw() {};

	void addedToEntity()
	{
		requestMessage("PickUpOrDown", &Throw::pickUpOrDown);
		requestMessage("PickUp", &Throw::pickUp);
		requestMessage("Throw", &Throw::Throwing);
		requestMessage("StopThrow", &Throw::stopThrowing);
		requestMessage("DropThrowable", &Throw::dropThrowable);
		requestMessage("isDizzy", &Throw::setIsDizzy);
		requestMessage("notDizzy", &Throw::setNotDizzy);

	}

	//for throw and throwing
	bool getToPickUp()
	{
		return this->toPickUp;
	}

	void setToPickUp(bool state)
	{
		if (!isDizzy)
		{
			this->toPickUp = state;
		}
	}

	bool getToPutDown()
	{
		return this->toPutDown;
	}

	void setToPutDown(bool state)
	{		
		if (!isDizzy)
		{
			this->toPutDown = state;
		}
	}

	bool getIsHoldingThrowable()
	{
		return this->isHoldingThrowable;
	}

	void setIsHoldingThrowable(bool state)
	{		
		if (!isDizzy)
		{
			this->isHoldingThrowable = state;
		}
	}

	bool getToThrow()
	{
		return this->toThrow;
	}

	void setToThrow(bool state)
	{		
		if (!isDizzy)
		{
			this->toThrow = state;
		}
	}

	void setHoldingEntityId(EntityId id)
	{		
		if (!isDizzy)
		{
			this->holdingEntityId = id;
		}
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
	bool isDizzy;

	void printToAll(Message const & msg)
	{

	}
	void update(Message const & msg)
	{
		if (isDizzy)
		{
			this->holdingEntityId = -1;
			this->toPickUp = false;
			this->toPutDown = false;
			this->isHoldingThrowable = false;
			this->toThrow = false;
		}
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

	void pickUp(Message const & msg)
	{
		setToPickUp(true);
		setToPutDown(false);
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

	void setIsDizzy(Message const &msg)
	{
		this->isDizzy = true;
	}

	void setNotDizzy(Message const & msg)
	{
		this->isDizzy = false;
	}
};

#endif