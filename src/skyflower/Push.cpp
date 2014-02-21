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
	this->box = nullptr;
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
	//find box to push
	if (getOwner()->wall && !box)
	{
		if (getOwner()->wall->hasComponents("Box") && canPush)
		{
			box = getOwner()->wall;
			relativePos = getOwner()->returnPos() - box->returnPos();
		}
	}

	//push box
	bool isPushingBox = false;
	if (box)
	{
		Movement* mov = getOwner()->getComponent<Movement*>("Movement");

		//find walking direction
		Vec3 pos = getOwner()->returnPos();
		mov->update(dt);
		Vec3 dir = (pos - getOwner()->returnPos()).Normalize()*-1;
		getOwner()->updatePos(pos);
		//axis aligne
		dir.Y = 0;
		if (dir.X == 0 && dir.Z == 0)
			dir = Vec3();
		else if (abs(dir.X) > abs(dir.Z))
			dir = Vec3(copysign(1.0f, dir.X), 0.0f, 0.0f);
		else
			dir = Vec3(0.0f, 0.0f, copysign(1.0f, dir.Z));

		//find direction to box
		Vec3 boxDir = (box->returnPos() - pos).Normalize();
		//axis aligne
		boxDir.Y = 0;
		if (abs(boxDir.X) > abs(boxDir.Z))
			boxDir = Vec3(copysign(1.0f, boxDir.X), 0.0f, 0.0f);
		else
			boxDir = Vec3(0.0f, 0.0f, copysign(1.0f, boxDir.Z));
		
		//check if entity is holding
		bool canpush = true;
		Throw* throwcomp = getOwner()->getComponent<Throw*>("Throw");
		if (throwcomp)
		{
			if (throwcomp->getIsHoldingThrowable())
				canpush = false;
		}

		//release pusher from box
		if (!canpush || box->getComponent<BoxComp*>("Box")->isFalling() || (dir != Vec3() && ((dir != boxDir && !canDrag) || (canDrag && dir*boxDir == Vec3()))))
		{
			relativePos = Vec3();
			box = nullptr;
		}
		//push box
		else
		{
			// Rotate the entity to make it perpendicular to the box it is pushing.
			Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f);
			if (abs(boxDir.X) > abs(dir.Z))
				rotation.Y = DegreesToRadians(-90.0f * boxDir.X);
			else
				rotation.Y = DegreesToRadians(90.0f + 90.0f * boxDir.Z);

			//move box
			box->updatePos(box->returnPos() + dir*dt*box->getComponent<Movement*>("Movement")->GetSpeed());

			//move relative to box
			getOwner()->updatePos((box->returnPos() + relativePos)*Vec3(1, 0, 1) + pos*Vec3(0, 1, 0));

			//rotate pusher with box
			getOwner()->updateRot(Vec3(box->returnRot().X, rotation.Y, box->returnRot().Z));

			//play push animation for player
			if (dir != Vec3() && getOwnerId() == 1 && getOwner()->getAnimatedInstance())
			{
				isPushingBox = true;
				getOwner()->getAnimatedInstance()->SetAnimation(3, true);
			}
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


bool Push::isDraging()
{
	if (getOwner()->wall)
		return getOwner()->wall->hasComponents("Box") && canPush && canDrag;
	return false;;
}