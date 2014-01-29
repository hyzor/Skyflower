#include "shared/util.h"

#include "Components/Push.h"
#include "EntityManager.h"




void Push::update(float dt)
{
	//push box
	Entity *pusher = getOwner();
	Entity *pushedObject = pusher->wall;

	if (pushedObject != nullptr)
	{
		if (pushedObject->hasComponents("Box"))
		{
			Vec3 pushedObjectPos = pushedObject->returnPos();
			Vec3 dir = pushedObjectPos - pusher->returnPos();
			Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f);

			if (abs(dir.X) > abs(dir.Z))
			{
				float sign = copysign(1.0f, dir.X);
				dir = Vec3(sign, 0.0f, 0.0f);
				rotation.Y = DegreesToRadians(-90.0f * sign);
			}
			else
			{
				float sign = copysign(1.0f, dir.Z);
				dir = Vec3(0.0f, 0.0f, sign);
				rotation.Y = DegreesToRadians(90.0f + 90.0f * sign);
			}

			pushedObject->updatePos(pushedObjectPos + dir * dt * pushedObject->getComponent<Movement*>("Movement")->GetSpeed());
			pusher->updateRot(rotation);

			EntityId pusherId = getOwnerId();

			if (pusherId == 1 && pusher->getAnimatedInstance())
			{
				pusher->getAnimatedInstance()->SetAnimation(3);
			}
		}
	}


	//pushAll();
	
}

void Push::stopPush(Message const& msg)
{
	if (getOwner()->getPhysics()->GetStates()->isBeingPushed)
		getEntityManager()->sendMessageToEntity("stopBeingPushed", getOwnerId());
}

bool Push::canPush(Entity* target)
{
	return target->sphere->Test(*getOwner()->sphere);
}

void Push::push(Entity* target)
{

	//collision and pushing between two entities
	Entity* e = target;
	if (getOwner() != e)
	{
		if (getOwner()->sphere && e->sphere)
		{
			//are two entities colliding?
			if (canPush(e))
			{
				Vec3 dir = (e->returnPos() - getOwner()->returnPos()).Normalize();

				//if (this->fEntitys[i]->getType() == "player" || this->fEntitys[j]->getType() == "player")

				//if "i" is moving and can push, and that "j" is pushable
				if (getOwner()->getPhysics()->GetStates()->isMoving && e->hasComponents("Pushable"))
				{
					e->getPhysics()->SetPushDirection(dir * 10);
					getEntityManager()->sendMessageToEntity("beingPushed", e->fId);
				}
			}
		}
	}
}

void Push::pushAll()
{
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
		push(getEntityManager()->getEntity(getEntityManager()->getEntityId(j)));
}
