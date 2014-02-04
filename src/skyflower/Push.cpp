#include "shared/util.h"

#include "Components/Push.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

Push::Push() : Component("Push")
{
}

Push::~Push()
{
}

void Push::addedToEntity()
{
	m_isPushingBox = false;

	//requestMessage("inAir", &Push::stopPush);
	requestMessage("Wall", &Push::stopPush);
}

void Push::removeFromEntity()
{
}

void Push::update(float dt)
{
	//push box
	Entity *pusher = getOwner();
	Entity *pushedObject = pusher->wall;
	bool isPushingBox = false;

	if (pushedObject != nullptr)
	{
		if (pushedObject->hasComponents("Box"))
		{
			isPushingBox = true;

			Vec3 pushedObjectPos = pushedObject->returnPos();
			Vec3 pusherPos = pusher->returnPos();
			Vec3 dir = pushedObjectPos - pusherPos;

			if (!m_isPushingBox)
			{
				// Entity just started pushing box.
				m_initialPushDirection = dir;
				m_initialPushDirection.Y = 0.0f;
				m_initialPushDirection.Normalize();
			}

			// Make the entity "stick" to the box it is pushing.
			Vec3 tempDir = dir;
			tempDir.Y = 0;

			float oldPusherY = pusherPos.Y;
			pusherPos = pushedObjectPos - m_initialPushDirection * tempDir.Length();
			pusherPos.Y = oldPusherY;

			pusher->updatePos(pusherPos);

			// Rotate the entity to make it perpendicular to the box it is pushing.
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

	m_isPushingBox = isPushingBox;

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
		push(getEntityManager()->getEntityByIndex(j));
}
