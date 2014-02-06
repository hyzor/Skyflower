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
#include "Application.h"

// Must be included last!
#include "shared/debug.h"

#define JUMP_SPEED_FACTOR_LEFT 0.00625f
#define JUMP_SPEED_FACTOR_RIGHT 0.00625f
#define JUMP_SPEED_FACTOR_FORWARD 0.0005f
#define JUMP_SPEED_FACTOR_BACKWARD 0.0125f

static const char *fallingSounds[] = {
	"player/falling1.wav"
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
	this->timeFalling = 0.0f;
	this->mInitialJumpDir = None;
	this->mParticleSystem = NULL;
}

Movement::~Movement()
{
}

void Movement::addedToEntity() {
	this->p = getOwner()->getPhysics();

	this->mParticleSystem = getOwner()->getModules()->graphics->CreateParticleSystem();
	this->mParticleSystem->SetParticleType(ParticleType::PT_PARTICLE);
	this->mParticleSystem->SetParticleAgeLimit(0.25f);
	this->mParticleSystem->SetEmitFrequency(1.0f / 100.0f);

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

	getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystem);
	this->mParticleSystem = NULL;
}

void Movement::update(float deltaTime)
{
	Vec3 pos = getEntityPos();
	Vec3 rot = getEntityRot();
	p->Update(deltaTime);

	GravityComponent *gravity = getOwner()->getComponent<GravityComponent*>("Gravity");

	if (isInAir)
		timeFalling += deltaTime;

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
				getOwner()->getModules()->sound->PlaySound(GetPlayerSoundFile(fallingSounds[rand()  % ARRAY_SIZE(fallingSounds)]), 0.25f);
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
			if (std::abs((((int)targetRot) % 360) - ((int)walkAngle % 360)) > 1)
			{
				float tot = targetRot - walkAngle;
				if (tot > 180)
					tot -= 360;
				if (tot < -180)
					tot += 360;

				if (tot > 0)
					walkAngle += std::abs(tot) * deltaTime * 7;
				else
					walkAngle -= std::abs(tot) * deltaTime * 7;
			}
			else
				walkAngle = targetRot;

			p->GetStates()->isMoving = true;
			float totalSpeed = this->speed;

			if (this->p->GetStates()->isJumping && this->isInAir)
			{
				//DoJumpStuff(totalSpeed);
				p->RotateRelativeVec3(rot, this->camLook, targetRot);
				p->Walk(pos, totalSpeed * deltaTime);
			}
			if (!this->p->GetStates()->isJumping /*&& !this->isInAir*/ && this->p->GetVelocity().Y <= 0.0f)
			{
				p->RotateRelativeVec3(rot, this->camLook, targetRot);
				p->Walk(pos, totalSpeed * deltaTime);
			}

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
		Push *pushComponent = getOwner()->getComponent<Push *>("Push");

		if ((this->isInAir && timeFalling > 0.3f) || p->GetStates()->isJumping)
		{
			getOwner()->getAnimatedInstance()->SetAnimation(1, false);
		}
		else if (pushComponent && !pushComponent->isPushingBox())
		{
			if (p->GetStates()->isMoving)
			{
				getOwner()->getAnimatedInstance()->SetAnimation(0, true);
			}
			else
			{
				getOwner()->getAnimatedInstance()->SetAnimation(4, true);
			}
		}
	}

	this->p->ApplyVelocityToPos(pos);

	// Update particle system
	Vec3 emitDirection = Vec3(cosf(-rot.Y + 3.14f / 2), 0, sinf(-rot.Y + 3.14f / 2)).Normalize();
	float particleAcceleration = 10.0f;

	this->mParticleSystem->SetActive(!(this->isInAir || !p->GetStates()->isMoving));
	this->mParticleSystem->SetEmitPos(XMFLOAT3(pos.X, pos.Y, pos.Z));
	this->mParticleSystem->SetEmitDir(XMFLOAT3(emitDirection.X, emitDirection.Y, emitDirection.Z));
	this->mParticleSystem->SetConstantAccel(XMFLOAT3(emitDirection.X * particleAcceleration, emitDirection.Y * particleAcceleration, emitDirection.Z * particleAcceleration));

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
	this->timeFalling = 0.0f;
	this->isInAir = false;
	this->mInitialJumpDir = None;
}

void Movement::Jump(Message const& msg)
{
	if (!getOwner()->ground && timeFalling > 0.3f)
		return;

	Vec3 pos = getEntityPos();

	float forwardJumpSpeed = 0.0f;

	if (this->isMovingBackward || this->isMovingForward || this->isMovingLeft || this->isMovingRight)
	{
		forwardJumpSpeed = this->speed;
	}

	if (p->Jump(pos, forwardJumpSpeed))
	{
		//Remember the direction faced when starting the jump
		if (this->isMovingForward)
			this->mInitialJumpDir = Forward;
		else if (this->isMovingBackward)
			this->mInitialJumpDir = Backward;
		else if (this->isMovingLeft)
			this->mInitialJumpDir = Left;
		else if (this->isMovingRight)
			this->mInitialJumpDir = Right;
		else
			this->mInitialJumpDir = None;

		updateEntityPos(pos);

		Entity *owner = getOwner();
		GravityComponent *gravity = owner->getComponent<GravityComponent*>("Gravity");

		if (gravity)
		{
			gravity->setEnabled(false);
			this->timeUntilGravityEnable = MAX_JUMP_KEY_TIME;
		}

		owner->getModules()->sound->PlaySound(GetPlayerSoundFile("player/jump1.wav"), 1.0f, &pos.X);
	}
}

void Movement::StopJump(Message const& msg)
{
	this->timeUntilGravityEnable = 0.0f;
}


void Movement::DoJumpStuff(float &jSpeed)
{
	switch (this->mInitialJumpDir)
	{
	case None:
		jSpeed *= 0.01f;
		break;
	case Forward:
		if (this->isMovingForward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_FORWARD;
		}
		else if (this->isMovingBackward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_BACKWARD;
		}
		else if (this->isMovingLeft)
		{
			jSpeed *= JUMP_SPEED_FACTOR_LEFT;
		}
		else if (this->isMovingRight)
		{
			jSpeed *= JUMP_SPEED_FACTOR_RIGHT;
		}
		break;

	case Backward:
		if (this->isMovingForward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_BACKWARD;
		}
		else if (this->isMovingBackward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_FORWARD;
		}
		else if (this->isMovingLeft)
		{
			jSpeed *= JUMP_SPEED_FACTOR_RIGHT;
		}
		else if (this->isMovingRight)
		{
			jSpeed *= JUMP_SPEED_FACTOR_LEFT;
		}
		break;

	case Left:
		if (this->isMovingForward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_RIGHT;
		}
		else if (this->isMovingBackward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_LEFT;
		}
		else if (this->isMovingLeft)
		{
			jSpeed *= JUMP_SPEED_FACTOR_FORWARD;
		}
		else if (this->isMovingRight)
		{
			jSpeed *= JUMP_SPEED_FACTOR_BACKWARD;
		}
		break;

	case Right:
		if (this->isMovingForward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_LEFT;
		}
		else if (this->isMovingBackward)
		{
			jSpeed *= JUMP_SPEED_FACTOR_RIGHT;
		}
		else if (this->isMovingLeft)
		{
			jSpeed *= JUMP_SPEED_FACTOR_BACKWARD;
		}
		else if (this->isMovingRight)
		{
			jSpeed *= JUMP_SPEED_FACTOR_FORWARD;
		}
		break;
	}
}
