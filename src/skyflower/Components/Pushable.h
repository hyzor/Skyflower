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
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		//if you are being pushed or are moving from being pushed
		if (this->isPushed)
		{
			Vec3 entityPos = getEntityPos();
			p->movePushed(entityPos);
			this->updateEntityPos(this->p->movePushed(entityPos));
			float distance = (entityPos - this->startPos).Length();

			//if you have been pushed away far enough, entity is able to move again
			if (distance > this->maxDistance)
			{
				sendMessageToEntity(getOwnerId(), "StartMoving");
				this->isPushed = false;
			}
		}
	}

private:

	Physics *p;
	bool isPushed;
	Vec3 startPos;			//position the entity have when being pushed
	float maxDistance;		//the distance the entity will be pushed

	void beingPushed(Message const& msg)
	{
		cout << "BEING PUSHED" << endl;
		this->isPushed = true;
		this->startPos = getEntityPos();

		//stop the entity from moving, except for the push
		sendMessageToEntity(getOwnerId(), "StopMoving");
	}

};

#endif