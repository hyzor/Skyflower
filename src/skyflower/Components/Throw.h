#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include "Entity.h"
#include "Components/Throwable.h"
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

	Entity* getHeldEntity();
	
	void ThrowAt(Entity* e);
	void PutDown();
	void PickUp(Entity* e);
	void PickUpAll();

private:

	Vec3 throwTarget;

	bool toPickUp;
	Entity* heldEntity;
	Entity* aimEntity;
	bool isDizzy;

	void setAimVisibility(bool state);
	void pickUp(Message const & msg);
	void pickUpStop(Message const & msg);
	void Throwing(Message const & msg);
	void dropThrowable(Message const & msg);

	void setIsDizzy(Message const &msg);
	void setNotDizzy(Message const & msg);

	void updateAim();
	
};

#endif