#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include "Entity.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Throw : public Component {

public:
	Throw(bool haveAim);
	virtual ~Throw();

	void addedToEntity();
	void update(float dt);

	EntityId getHoldingEntityId();
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
	void setHoldingEntityId(EntityId id);

private:

	bool toPickUp;
	bool toPutDown;
	bool isHoldingThrowable;
	bool toThrow;
	EntityId holdingEntityId;
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