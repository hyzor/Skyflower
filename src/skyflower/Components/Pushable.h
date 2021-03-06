#ifndef PUSHABLE_H
#define PUSHABLE_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Pushable : public Component {

public:
	Pushable() : Component("Pushable")
	{
		this->isPushed = false;
		this->maxDistance = 40;
	}

	virtual ~Pushable()
	{
	}

	void addedToEntity()
	{
		this->p = getOwner()->getPhysics();
		requestMessage("beingPushed", &Pushable::beingPushed);
		requestMessage("stopBeingPushed", &Pushable::stopBeingPushed);
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		//if you are being pushed or are moving from being pushed
		if (this->isPushed)
		{
			Vec3 entityPos = getOwner()->returnPos();
			p->MovePushed(entityPos);
			getOwner()->updatePos(this->p->MovePushed(entityPos));
			float distance = (entityPos - this->startPos).Length();

			//if you have been pushed away far enough, entity is able to move again
			if (distance > this->maxDistance || getOwner()->getComponent<Movement*>("Movement")->getTimeFalling() > 0.3f || getOwner()->wall)
			{
				getOwner()->sendMessage("StartMoving", this);
				this->isPushed = false;
				this->p->GetStates()->isBeingPushed = false;
			}
		}
	}

	bool getIsBeingPushed()
	{
		return this->isPushed;
	}

private:

	PhysicsEntity *p;
	bool isPushed;
	Vec3 startPos;			//position the entity have when being pushed
	float maxDistance;		//the distance the entity will be pushed

	void beingPushed(Message const& msg)
	{
		if (!this->isPushed)
		{
			//cout << "BEING PUSHED" << endl;
			this->isPushed = true;
			this->startPos = getOwner()->returnPos();
			this->p->GetStates()->isBeingPushed = true;

			//stop the entity from moving, except for the push
			getOwner()->sendMessage("StopMoving", this);
			getOwner()->sendMessage("DropThrowable", this);
		}
	}

	void stopBeingPushed(Message const& msg)
	{
		this->isPushed = false;
		this->p->GetStates()->isBeingPushed = false;
		getOwner()->sendMessage("StartMoving", this);
	}
};

#endif