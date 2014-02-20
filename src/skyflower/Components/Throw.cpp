#include "shared/util.h"
#include "Components/Throw.h"
#include "EntityManager.h"
//#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

Throw::Throw(bool haveAim) : Component("Throw")
{
	this->holdingEntityId = -1;
	this->toPickUp = false;
	this->toPutDown = false;
	this->isHoldingThrowable = false;
	this->toThrow = false;
	this->isDizzy = false;
	this->haveAim = haveAim;
}

Throw::~Throw()
{
}

void Throw::addedToEntity()
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

void Throw::update(float dt)
{
	if (isDizzy)
	{
		this->holdingEntityId = -1;
		this->toPickUp = false;
		this->toPutDown = false;
		this->isHoldingThrowable = false;
		this->toThrow = false;
	}
	//sendMessageToEntity(this->getOwnerId(), "update");
}

EntityId Throw::getHoldingEntityId()
{
	return this->holdingEntityId;
}

bool Throw::getToThrow()
{
	return this->toThrow;
}

bool Throw::getIsHoldingThrowable()
{
	return this->isHoldingThrowable;
}

bool Throw::getToPickUp()
{
	return this->toPickUp;
}

bool Throw::getToPutDown()
{
	return this->toPutDown;
}

bool Throw::getIsDizzy()
{
	return isDizzy;
}

void Throw::setToPickUp(bool state)
{
	this->toPickUp = state;
}

void Throw::setToPutDown(bool state)
{
	this->toPutDown = state;
}

bool Throw::getHaveAim()
{
	return this->haveAim;
}

void Throw::setHaveAim(bool state)
{
	this->haveAim = state;
}

void Throw::setIsHoldingThrowable(bool state)
{
	this->isHoldingThrowable = state;
}

void Throw::setToThrow(bool state)
{
	this->toThrow = state;
}

void Throw::setHoldingEntityId(EntityId id)
{
	this->holdingEntityId = id;
}

void Throw::dropThrowable(Message const & msg)
{
	if (isHoldingThrowable)
	{
		setToPutDown(false);
		setIsHoldingThrowable(false);
		setToPickUp(false);
		sendMessageToEntity(holdingEntityId, "Dropped");
		sendMessageToEntity(getOwnerId(), "isNotHoldingThrowable");
		if (haveAim)
		{
			getEntityManager()->updateEntityVisibility(false, 5000);
		}
	}
}

void Throw::setAimVisibility(bool state)
{
	getEntityManager()->updateEntityVisibility(state, 5000);
}

//when pick up or put down-button is pressed
void Throw::pickUpOrDown(Message const & msg)
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

void Throw::pickUp(Message const & msg)
{
	setToPickUp(true);
	setToPutDown(false);
}

void Throw::pickUpStop(Message const & msg)
{
	setToPickUp(false);
}

//if the entity should throw something away
void Throw::Throwing(Message const & msg)
{
	setToThrow(true);
}

//if the entity should stop to try to throw something.
void Throw::stopThrowing(Message const & msg)
{

	setToThrow(false);

}

void Throw::setIsDizzy(Message const &msg)
{
	this->isDizzy = true;
}

void Throw::setNotDizzy(Message const & msg)
{
	this->isDizzy = false;
}