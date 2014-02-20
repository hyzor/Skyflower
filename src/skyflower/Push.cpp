#include "shared/util.h"

#include "Components/Push.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

Push::Push() : Component("Push")
{
	this->canPush = true;
	this->isColliding = false;
	this->resetSpeed = false;
	this->canDrag = false;
}

Push::~Push()
{
}

void Push::addedToEntity()
{
	m_isPushingBox = false;

	//requestMessage("inAir", &Push::stopPush);
	requestMessage("Wall", &Push::stopPush);
	requestMessage("isHoldingThrowable", &Push::setCanNotPush);
	requestMessage("isNotHoldingThrowable", &Push::setCanPush);
	requestMessage("beingPushed", &Push::beingPushed);
	requestMessage("PickUpStart", &Push::pickUpStart);
	requestMessage("PickUpStop", &Push::pickUpStop);
}

void Push::removeFromEntity()
{
}

void Push::update(float dt)
{
	//push box
	Entity* pusher = getOwner();
	Entity* pushedObject = pusher->wall;
	bool isPushingBox = false;

	if (pushedObject)
	{
		if (pushedObject->hasComponents("Box") && canPush)
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

			//check if entity is holding
			bool p = true;
			Throw* throwcomp = pusher->getComponent<Throw*>("Throw");
			if (throwcomp)
			if (throwcomp->getIsHoldingThrowable())
				p = false;

			if (p)
			{
				if (canDrag)
				{
					pushedObject->updatePos(pushedObjectPos - dir * dt * pushedObject->getComponent<Movement*>("Movement")->GetSpeed());
				}
				else
				{
					pushedObject->updatePos(pushedObjectPos + dir * dt * pushedObject->getComponent<Movement*>("Movement")->GetSpeed());
				}
			}

					

			pusher->updateRot(rotation);

			getOwner()->updateRot(Vec3(pushedObject->returnRot().X, getOwner()->returnRot().Y, pushedObject->returnRot().Z));

			if (getOwnerId() == 1 && pusher->getAnimatedInstance())
				pusher->getAnimatedInstance()->SetAnimation(3, true);

		}
	}

	m_isPushingBox = isPushingBox;


	//reset the entity to normal speed
	if (resetSpeed)
	{
		Movement* mov = getOwner()->getComponent<Movement*>("Movement");
		mov->SetSpeed(mov->GetSpeed()+80*dt); //accelerate speed 40*dt
		if (mov->GetSpeed() >= pSpeed)
		{
			resetSpeed = false;
			mov->SetSpeed(pSpeed);
		}
	}

	//pushAll();

	isColliding = false;

}

void Push::stopPush(Message const& msg)
{
	if (getOwner()->getPhysics()->GetStates()->isBeingPushed)
		getEntityManager()->sendMessageToEntity("stopBeingPushed", getOwnerId());
}

bool Push::colliding(Entity* target)
{
	if (!target->getPhysics()->GetStates()->isBeingPushed && (target->sphere->Test(*getOwner()->sphere) || isColliding))
	{
		isColliding = false;
		//get owner look vector
		Vec3 rot = getOwner()->returnRot();
		Vec3 look = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize();
		look.Normalize();

		//get direction to target
		Vec3 dir = target->returnPos() - getOwner()->returnPos();
		dir.Y = 0;
		dir.Normalize();

		//se if owner looks at target
		float dot = look.Dot(dir);
		if (dot > 0.6f)
			return true;
	}

	return false;
}

void Push::push(Entity* target)
{
	//collision and pushing between two entities
	Entity* e = target;
	if (getOwner() != e)
	{
		if (getOwner()->sphere && e->sphere)
		{
			//are two entities colliding, can they push and are they not dizzy
			if (colliding(e) && canPush && !getOwner()->getComponent<Movement*>("Movement")->getIsDizzy())
			{
				Vec3 dir = (e->returnPos() - getOwner()->returnPos());
				dir.Y = 0;
				dir.Normalize();

				//if this entity is moving and can push, and that the other entity is pushable
				if (getOwner()->getPhysics()->GetStates()->isMoving && e->hasComponents("Pushable"))
				{
					e->getPhysics()->SetPushDirection(dir * 10);
					getEntityManager()->sendMessageToEntity("beingPushed", e->fId);

					Vec3 position = e->returnPos();
					e->getModules()->sound->PlaySound("push.wav", 1.0f, &position.X);

					//slow entity for 1 sec after push
					Movement* mov = getOwner()->getComponent<Movement*>("Movement");
					if (mov)
					{
						pSpeed = mov->GetSpeed();
						mov->SetSpeed(5);
						resetSpeed = true;
					}
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

bool Push::isPushingBox()
{
	return m_isPushingBox;
}

void Push::setCanPush(Message const& msg)
{
	this->canPush = true;
}

void Push::setCanNotPush(Message const& msg)
{
	this->canPush = false;
}

void Push::beingPushed(Message const& msg)
{
	this->isColliding = true;
}

void Push::pickUpStart(Message const& msg)
{
	canDrag = true;
}

void Push::pickUpStop(Message const& msg)
{
	canDrag = false;
}