#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include "Entity.h"
#include "Components/Throwable.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

#define THROW_FORCE 150.0f

class Throw : public Component {

public:
	Throw(bool haveAim);
	virtual ~Throw();

	void addedToEntity();
	void removeFromEntity();

	void update(float dt);

	Entity* getHeldEntity();
	bool getToThrow();
	bool getIsHoldingThrowable();
	bool getToPickUp();
	bool getToPutDown();
	bool getIsDizzy();
	void setToPickUp(bool state);
	void setToPutDown(bool state);
	bool getHaveAim();
	void setHaveAim(bool state);
	void setIsHoldingThrowable(bool state);
	void setToThrow(bool state);

private:

	bool toPickUp;
	bool toPutDown;
	bool isHoldingThrowable;
	bool toThrow;
	Entity* heldEntity;
	Throwable* heldEntityThrowable;
	Entity* aimEntity;
	bool isDizzy;
	bool haveAim;
	void setAimVisibility(bool state);
	void pickUpOrDown(Message const & msg);
	void pickUp(Message const & msg);
	void pickUpStop(Message const & msg);
	void Throwing(Message const & msg);
	void stopThrowing(Message const & msg);
	void dropThrowable(Message const & msg);
	void setIsDizzy(Message const &msg);
	void setNotDizzy(Message const & msg);
	
};

#endif