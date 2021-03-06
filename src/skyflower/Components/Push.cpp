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
	requestMessage("beingPushed", &Push::beingPushed);
	requestMessage("PickUpStart", &Push::pickUpStart);
	requestMessage("PickUpStop", &Push::pickUpStop);
}

void Push::removeFromEntity()
{
}

void Push::update(float dt)
{
	this->canPush = true;
	if (getOwner()->hasComponents("Pushable"))
	{
		if (getOwner()->getComponent<Pushable*>("Pushable")->getIsBeingPushed())
			this->canPush = false;
	}
	if (getOwner()->hasComponents("Throw"))
	{
		if (getOwner()->getComponent<Throw*>("Throw")->getHeldEntity())
			this->canPush = false;
	}
	if (getOwner()->hasComponents("Movement"))
	{
		if (getOwner()->getComponent<Movement*>("Movement")->getIsDizzy())
			this->canPush = false;
		if (getOwner()->getComponent<Movement*>("Movement")->getIsInAir())
			this->canPush = false;
	}


	//find box to push or pull
	if (getOwner()->wall && !box)
	{
		if (getOwner()->wall->hasComponents("Box") && getOwner()->hasComponents("Movement") && getOwner()->wall->hasComponents("Movement") && canPush)
		{
			if (getOwner()->wall->getComponent<BoxComp*>("Box")->getCanBeMoved())
			{
				box = getOwner()->wall;
				relativePos = getOwner()->returnPos() - box->returnPos();
			}
		}
	}

	//push or pull box 
	bool isPushingBox = false;
	if (box)
	{
		//find walking direction
		Vec3 pos = getOwner()->returnPos();
		getOwner()->getComponent<Movement*>("Movement")->update(dt);
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
			if (throwcomp->getHeldEntity())
				canpush = false;
		}

		//normal box
		BoxComp* boxComp = box->getComponent<BoxComp*>("Box");
		if (boxComp->MinDist() == 0)
		{
			//release pusher from box
			if (!canpush || box->getComponent<BoxComp*>("Box")->isFalling() || (dir != Vec3() && ((dir != boxDir && !canDrag) || (canDrag && dir*boxDir == Vec3()))) || (canDrag && getOwner()->wall && getOwner()->wall != box))
			{
				relativePos = Vec3();
				box = nullptr;
			}
		}
		//box puzzel box
		else
		{
			//stop pusher from jumping while pushing
			getOwner()->getComponent<Movement*>("Movement")->canJump = false;

			//position in grid
			Vec3 fromStart = boxComp->startPos - box->returnPos()*Vec3(1, 0, 1);
			fromStart /= boxComp->MinDist();
			Vec3 to = Vec3(roundf(fromStart.X), 0, roundf(fromStart.Z));

			//box at right position
			if ((fromStart - to).Length() < 0.08f)
			{
				//release box
				if (!boxComp->getCanBeMoved() || !canpush || box->getComponent<BoxComp*>("Box")->isFalling() || (dir != Vec3() && ((dir != boxDir && !canDrag) || (canDrag && (dir != boxDir*-1 && dir != boxDir)))))
				{
					getOwner()->getComponent<Movement*>("Movement")->canJump = true;
					box->updatePos(boxComp->startPos - to*boxComp->MinDist() + Vec3(0, box->returnPos().Y, 0));
					relativePos = Vec3();
					box = nullptr;
				}
			}
			else
			{
				//limit movement to backwards and forwards
				if (dir*boxDir == Vec3())
					dir = Vec3();

				//move to closes grid position
				if (dir == Vec3())
				{
					dir = (fromStart - to).Normalize();
					if (abs(dir.X) > abs(dir.Z))
						dir = Vec3(copysign(1.0f, dir.X), 0.0f, 0.0f);
					else
						dir = Vec3(0.0f, 0.0f, copysign(1.0f, dir.Z));
				}

				//disable drag into wall
				if (dir == boxDir*-1 && (getOwner()->wall && getOwner()->wall != box))
					dir = Vec3();
			}
		}

		//push box
		if (box)
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
			if (dir != Vec3() && getOwnerId() == 1 && getOwner()->IsAnimated())
			{
				isPushingBox = true;
				getOwner()->SetAnimation(3, true, true, true);
			}
		}
	}

	m_isPushingBox = isPushingBox;


	//reset the entity to normal speed
	if (resetSpeed)
	{
		currSpeed += 80 * dt; //accelerate speed
		if (currSpeed >= pSpeed)
		{
			resetSpeed = false;
			currSpeed = pSpeed;
		}
		getOwner()->getComponent<Movement*>("Movement")->SetSpeed(currSpeed);
	}

	//pushAll();

	isColliding = false;

}

void Push::stopPush(Message const& msg)
{
	if (getOwner()->getPhysics()->GetStates()->isBeingPushed)
		getOwner()->sendMessage("stopBeingPushed", this);
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
					e->sendMessage("beingPushed", this);

					Vec3 position = e->returnPos();
					e->getModules()->sound->PlaySound("push.wav", 1.0f, &position.X);

					//slow entity for 1 sec after push
					Movement* mov = getOwner()->getComponent<Movement*>("Movement");
					if (mov)
					{
						if (!resetSpeed)
						{
							pSpeed = mov->GetSpeed();
							mov->SetSpeed(5);
							currSpeed = mov->GetSpeed();
							resetSpeed = true;
						}
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

bool Push::isResettingSpeed()
{
	return resetSpeed;
}