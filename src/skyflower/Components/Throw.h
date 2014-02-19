#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Throw : public Component {

public:

	Throw(bool haveAim) : Component("Throw")
	{
		this->holdingEntityId = -1;
		this->toPickUp = false;
		this->toPutDown = false;
		this->isHoldingThrowable = false;
		this->toThrow = false;
		this->isDizzy = false;
		this->haveAim = haveAim;
	}
	virtual ~Throw() {};

	void addedToEntity()
	{
		requestMessage("PickUpOrDown", &Throw::pickUpOrDown);
		requestMessage("PickUp", &Throw::pickUp);
		requestMessage("PickUpStart", &Throw::pickUp);
		requestMessage("PickUpStop", &Throw::pickUpStop);
		requestMessage("Throw", &Throw::Throwing);
		requestMessage("StopThrow", &Throw::stopThrowing);
		requestMessage("DropThrowable", &Throw::dropThrowable);
		requestMessage("isDizzy", &Throw::setIsDizzy);
		requestMessage("notDizzy", &Throw::setNotDizzy);

	}

	

	void update(float dt)
	{
		if (isDizzy)
		{
			/*this->holdingEntityId = -1;
			this->toPickUp = false;
			this->toPutDown = false;
			this->isHoldingThrowable = false;
			this->toThrow = false;*/
		}
		//sendMessageToEntity(this->getOwnerId(), "update");
	}
	

	EntityId getHoldingEntityId()
	{
		return this->holdingEntityId;
	}

	bool getToThrow()
	{
		return this->toThrow;
	}

	bool getIsHoldingThrowable()
	{
		return this->isHoldingThrowable;
	}

	bool getToPickUp()
	{
		return this->toPickUp;
	}

	bool getToPutDown()
	{
		return this->toPutDown;
	}

	bool IsDizzy()
	{
		return isDizzy;
	}


	void setToPickUp(bool state)
	{
		this->toPickUp = state;
	}

	void setToPutDown(bool state)
	{
		this->toPutDown = state;
	}

	bool getHaveAim()
	{
		return this->haveAim;
	}

	void setHaveAim(bool state)
	{
		this->haveAim = state;
	}

	void setIsHoldingThrowable(bool state)
	{
		this->isHoldingThrowable = state;
	}

	void setToThrow(bool state)
	{
		this->toThrow = state;
	}

	void setHoldingEntityId(EntityId id)
	{
		this->holdingEntityId = id;
	}

private:

	bool toPickUp;
	bool toPutDown;
	bool isHoldingThrowable;
	bool toThrow;
	EntityId holdingEntityId;
	bool isDizzy;
	bool haveAim;

	void printToAll(Message const & msg)
	{

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

	void pickUpStop(Message const & msg)
	{
		setToPickUp(false);
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