#include <string>
#include <iostream>
#include <sstream>

#include "shared/Vec3.h"
#include "shared/util.h"
#include "physics/PhysicsEngine.h"
#include "physics/Collision.h"

#include "Movement.h"
#include "Entity.h"
#include "Health.h"
#include "Gravity.h"

static const char *fallingSounds[] = {
	"player/wilhelm_scream.wav",
	"quake/falling1.wav"
};

Movement::Movement(float speed) : Component("Movement")
{
	this->isMovingForward = false;
	this->isMovingBackward = false;
	this->isMovingLeft = false;
	this->isMovingRight = false;
	this->canMove = true;
	this->camLook = Vec3(0.0f, 0.0f, 0.0f);
	this->speed = speed;
	this->targetRot = 0.0f;
	this->walkAngle = 0.0f;
}

Movement::~Movement()
{
}

void Movement::addedToEntity() {
	this->p = getOwner()->getPhysics();

	requestMessage("StartMoveForward", &Movement::startMoveForward);
	requestMessage("StartMoveBackward", &Movement::startMoveBackward);
	requestMessage("StartMoveLeft", &Movement::startMoveLeft);
	requestMessage("StartMoveRight", &Movement::startMoveRight);
	requestMessage("StopMoveForward", &Movement::stopMoveForward);
	requestMessage("StopMoveBackward", &Movement::stopMoveBackward);
	requestMessage("StopMoveLeft", &Movement::stopMoveLeft);
	requestMessage("StopMoveRight", &Movement::stopMoveRight);
		
	//used when pushed
	requestMessage("StartMoving", &Movement::startMoving);
	requestMessage("StopMoving", &Movement::stopMoving);

	requestMessage("inAir", &Movement::inAir);
	requestMessage("notInAir", &Movement::notInAir);

	requestMessage("Jump", &Movement::Jump);
	requestMessage("StopJump", &Movement::StopJump);
}

void Movement::removeFromEntity()
{
	this->p = NULL;
}

void Movement::update(float deltaTime)
{
	Vec3 pos = getEntityPos();
	Vec3 rot = getEntityRot();
	p->Update(deltaTime);
		
	GravityComponent *gravity = getOwner()->getComponent<GravityComponent*>("Gravity");

	if (gravity && !gravity->isEnabled())
	{
		if (this->timeUntilGravityEnable > 0.0f)
			this->timeUntilGravityEnable -= deltaTime;

		if (this->timeUntilGravityEnable <= 0.0f)
			gravity->setEnabled(true);
	}

	Health *health = getOwner()->getComponent<Health*>("Health");

	if (health)
	{
		if (pos.Y < -100)
		{
			health->setHealth(0);
				
			if (getOwnerId() == 1)
			{
				float soundPosition[3] = { 0.0f, 0.0f, 0.0f };
				getOwner()->getModules()->sound->PlaySound(fallingSounds[rand()  % ARRAY_SIZE(fallingSounds)], soundPosition, 0.25f, true);
			}
		}

		if (!health->isAlive())
		{
			sendMessageToEntity(this->getOwnerId(), "Respawn");
			p->SetVelocity(Vec3(0, 0, 0));
			health->setHealth(100);
			return;
		}
	}

	//float walkAngle = 0.0f;

	if (canMove)
	{
		if (this->isMovingForward) {
			if (this->isMovingLeft) {
				targetRot = -45.0f;
			}
			else if (this->isMovingRight) {
				targetRot = 45.0f;
			}
			else {
				targetRot = 0.0f;
			}
		}
		else if (this->isMovingBackward) {
			if (this->isMovingLeft) {
				targetRot = -90.0f - 45.0f;
			}
			else if (this->isMovingRight) {
				targetRot = 180.0f - 45.0f;
			}
			else {
				targetRot = 180.0f;
			}
		}
		else if (this->isMovingLeft) {
			targetRot = -90.0f;
		}
		else if (this->isMovingRight) {
			targetRot = 90.0f;
		}

		if (isMovingBackward || isMovingForward || isMovingLeft || isMovingRight)
		{
			if (std::abs(targetRot - walkAngle) > 1.0f)
			{
				float tot = targetRot - walkAngle;
				if (tot > 180)
					tot -= 360;
				if (tot < -180)
					tot += 360;

				if (tot > 0)
					walkAngle += deltaTime * 750;
				else
					walkAngle -= deltaTime * 750;
			}
			else
				walkAngle = targetRot;
			p->GetStates()->isMoving = true;
			p->MoveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, targetRot);

			// If the player is moving, rotate it to match the camera's direction.
			if (getOwnerId() == 1)
			{
				rot = Vec3(0.0f, -this->yaw + DegreesToRadians(90.0f + walkAngle), 0.0f);
			}
		}
		else
		{
			p->GetStates()->isMoving = false;
		}
	}

	if (getOwnerId() == 1 && getOwner()->getAnimatedInstance())
	{
		if (this->isInAir)
		{
			getOwner()->getAnimatedInstance()->SetAnimation(1);
		}
		else
		{
			if (p->GetStates()->isMoving)
			{
				getOwner()->getAnimatedInstance()->SetAnimation(0);
			}
			else
			{
				getOwner()->getAnimatedInstance()->SetAnimation(4);
			}
		}
	}

	updateEntityPos(pos);
	updateEntityRot(rot);
}

void Movement::setCamera(Vec3 look, Vec3 right, Vec3 up)
{
	if (p)
	{
		this->camLook = look;
	}
}

void Movement::setYaw(float yaw)
{
	this->yaw = yaw;
}

void Movement::moveforward()
{
	this->isMovingForward = true;
}

float Movement::GetSpeed()
{
	return speed;
}

void Movement::SetSpeed(float speed)
{
	this->speed = speed;
}

void Movement::startMoveForward(Message const& msg)
{
	this->isMovingForward = true;
}

void Movement::startMoveBackward(Message const& msg)
{
	this->isMovingBackward = true;
}

void Movement::startMoveLeft(Message const& msg)
{
	this->isMovingLeft = true;
}

void Movement::startMoveRight(Message const& msg)
{
	this->isMovingRight = true;
}

void Movement::stopMoveForward(Message const& msg)
{
	this->isMovingForward = false;
}

void Movement::stopMoveBackward(Message const& msg)
{
	this->isMovingBackward = false;
}

void Movement::stopMoveLeft(Message const& msg)
{
	this->isMovingLeft = false;
}

void Movement::stopMoveRight(Message const& msg)
{
	this->isMovingRight = false;
}

void Movement::stopMoving(Message const& msg)
{
	this->canMove = false;
}

void Movement::startMoving(Message const& msg)
{
	this->canMove = true;
}

void Movement::inAir(Message const& msg)
{
	this->isInAir = true;
}

void Movement::notInAir(Message const& msg)
{
	this->isInAir = false;
}

void Movement::Jump(Message const& msg)
{
	if (!getOwner()->ground)
		return;

	Vec3 pos = getEntityPos();

	if (p->Jump(pos))
	{
		updateEntityPos(pos);

		Entity *owner = getOwner();
		GravityComponent *gravity = owner->getComponent<GravityComponent*>("Gravity");

		if (gravity)
		{
			gravity->setEnabled(false);
			this->timeUntilGravityEnable = MAX_JUMP_KEY_TIME;
		}

		owner->getModules()->sound->PlaySound("player/jump1.wav", &pos.X, 1.0f);
	}
}

void Movement::StopJump(Message const& msg)
{
	this->timeUntilGravityEnable = 0.0f;
}
