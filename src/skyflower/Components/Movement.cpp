#include <string>
#include <iostream>
#include <sstream>

#define _USE_MATH_DEFINES
#include <cmath>

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

#define RUN_PARTICLE_EMIT_RATE (1.0f / 10.0f)

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
	this->dizzyMaxTimer = 2.0f;
	this->isDizzy = false;
	this->yaw = 0;
	this->respawnTimer = 0;
	this->turnAngle = 0.0f;
	this->canJump = true;
	this->idleTimer = 0.0f;
	this->mParticleSystemRun = NULL;
	this->mParticleSystemDizzy = NULL;
	mParticleSystemDizzyAngle = 0.0f;
}

Movement::~Movement()
{
}

void Movement::addedToEntity() {
	this->p = getOwner()->getPhysics();

	this->mParticleSystemRun = getOwner()->getModules()->graphics->CreateParticleSystem();
	this->mParticleSystemRun->SetActive(true);
	this->mParticleSystemRun->SetParticleType(ParticleType::PT_DUST);
	this->mParticleSystemRun->SetParticleAgeLimit(0.25f);
	this->mParticleSystemRun->SetEmitFrequency(FLT_MAX);
	this->mParticleSystemRun->SetRandomVelocity(XMFLOAT3(20.0f, 5.0f, 20.0f));
	this->mParticleSystemRun->SetScale(XMFLOAT2(3.0f, 3.0f));

	this->mParticleSystemDizzy = getOwner()->getModules()->graphics->CreateParticleSystem();
	this->mParticleSystemDizzy->SetActive(false);
	this->mParticleSystemDizzy->SetParticleType(ParticleType::PT_BIRD);
	this->mParticleSystemDizzy->SetParticleAgeLimit(3.75f);
	this->mParticleSystemDizzy->SetEmitFrequency(1.0f / 2.0f);
	this->mParticleSystemDizzy->SetScale(XMFLOAT2(2.5f, 2.5f));

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

	requestMessage("isDizzy", &Movement::setIsDizzy);
}

void Movement::removeFromEntity()
{
	this->p = NULL;

	getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystemRun);
	getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystemDizzy);
	this->mParticleSystemRun = NULL;
	this->mParticleSystemDizzy = NULL;
}

void Movement::update(float deltaTime)
{
	Vec3 pos = getOwner()->returnPos();
	Vec3 rot = getOwner()->returnRot();
	float finalTargetRot = XMConvertToDegrees(-yaw);
	p->Update(deltaTime);

	GravityComponent *gravity = getOwner()->getComponent<GravityComponent*>("Gravity");
	AI* ai = getOwner()->getComponent<AI *>("AI");

	if (isInAir && getOwner()->sphere && getOwnerId() == 1)
	{
		timeFalling += deltaTime;
		for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
		{
			Entity* entity = getEntityManager()->getEntityByIndex(j);

			if (entity->sphere && entity->hasComponents("AI"))
			{
				if (entity->sphere->Test(*getOwner()->sphere))
				{
					Vec3 AIPos = entity->returnPos();
					Vec3 entityPos = getOwner()->returnPos();

					if (entityPos.Y-5 > AIPos.Y)
						entity->sendMessage("isDizzy", this);
				}
			}
		}
	}

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
		}

		if (!health->isAlive())
		{
			if (this->respawnTimer < 0.2)
			{
				this->respawnTimer += deltaTime;
			}
			else
			{
				getOwner()->sendMessage("Respawn", this);
				if (getOwnerId() == 1)
				{
					getOwner()->getModules()->sound->PlaySound(GetPlayerSoundFile(fallingSounds[rand() % ARRAY_SIZE(fallingSounds)]), 0.05f);
				}
				this->respawnTimer = 0.0f;
				return;
			}

		}
	}

	//float walkAngle = 0.0f;
	if (isDizzy)
	{
		dizzyTimer(deltaTime);
	}
	else if (canMove)
	{
		float oldTargetRot = targetRot;
		bool backwards = isMovingBackward;
		//Push* push = getOwner()->getComponent<Push*>("Push");
		//if (push)
		//{
			//if (push->isDraging())
				//backwards = false;
		//}

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
		else if (backwards) {
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

		finalTargetRot += targetRot;
		while (finalTargetRot < -180)
			finalTargetRot += 360;
		while (finalTargetRot > 180)
			finalTargetRot -= 360;

		if (backwards || isMovingForward || isMovingLeft || isMovingRight)
		{
			// Update player and AI rotation.
			float currentRot = XMConvertToDegrees(rot.Y)-90;

			while (currentRot < -180)
				currentRot += 360;
			while (currentRot > 180)
				currentRot -= 360;

			float fR1 = currentRot - finalTargetRot;
			float fR2 = finalTargetRot - currentRot;
			if (fR1 < 0.0f)
				fR1 += 360.0f;
			if (fR2 < 0.0f)
				fR2 += 360.0f;

			if (fR2 < fR1)
				currentRot += fR2 * 14 * deltaTime;
			else
				currentRot -= fR1 * 14 * deltaTime;

			rot = Vec3(0.0f, DegreesToRadians(currentRot + 90), 0.0f);
			getOwner()->updateRot(rot);





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
		}
		else
		{
			p->GetStates()->isMoving = false;
		}
	}

	if (getOwner()->IsAnimated())
	{
		if (getOwnerId() == 1)
		{

			if (isDizzy)
			{
				// Dizzy
				getOwner()->SetAnimation(10, true);
			}
			else if ((this->isInAir && timeFalling > 0.3f) || p->GetStates()->isJumping)
			{
				if (!getOwner()->IsPlayingAnimation(8) && !getOwner()->IsPlayingAnimation(9))
				{
					if (!getOwner()->IsPlayingAnimation(1) || (getOwner()->IsPlayingAnimation(1) && getOwner()->IsAnimationDone()))
					{
						// Raise hands for fall
						getOwner()->SetAnimation(8, false, false, true);
					}
				}

				if (getOwner()->IsPlayingAnimation(8) && getOwner()->IsAnimationDone())
				{
					// Fall
					getOwner()->SetAnimation(9, true);
				}
			}
			else
			{
				Push* push = getOwner()->getComponent<Push*>("Push");
				if (push && push->isPushingBox())
				{
				}
				else if (p->GetStates()->isMoving)
				{
					// Run
					if (getOwner()->IsPlayingAnimation(7, false, true) && !getOwner()->IsAnimationDone(false, true))
						getOwner()->SetAnimation(0, true, true, false);
					else
						getOwner()->SetAnimation(0, true, true, true);
					getOwner()->SetAnimationSpeed(0, speed / 25.0f, true, false);
				}
				else
				{
					// Idle
					if (idleTimer > 2.5f)
					{
						if (rand() % 10 < 6)
						{
							getOwner()->SetAnimationSpeed(4, 0.85f);
							getOwner()->SetAnimation(4, false);
						}
						idleTimer = 0.0f;
					}

					if (!getOwner()->IsPlayingAnimation(4, true, false) || (getOwner()->IsPlayingAnimation(4, true, false) && getOwner()->IsAnimationDone())) //if idle animation is done
					{
						getOwner()->SetAnimation(11, false);
					}
					idleTimer += deltaTime;
				}

				Throw* throwComponent = getOwner()->getComponent<Throw*>("Throw");
				if (throwComponent && throwComponent->getHeldEntity())
				{
					// Holding ball
					getOwner()->SetAnimation(6, true, false, true);
				}
			}
		}

		else if (ai)
		{
			

			// AI animations
			if (p->GetStates()->isJumping)
			{
				// Playing jump animation, do nothing.
			}
			else if (p->GetStates()->isMoving)
			{
				// Run
				if(getOwner()->IsPlayingAnimation(4, false, true) && !getOwner()->IsAnimationDone(false, true))
					getOwner()->SetAnimation(0, true, true, false);
				else
					getOwner()->SetAnimation(0, true, true, true);
			}
			else
			{
				// Idle

				// AIn har ingen idle animation, spela springanimationen istället.
				//getOwner()->SetAnimation(0, true, true, false);
			}

			Throw *throwComponent = getOwner()->getComponent<Throw *>("Throw");
			if (throwComponent && throwComponent->getHeldEntity())
			{
				// Holding ball
				getOwner()->SetAnimation(6, true, false, true);
			}
		}

	}

	this->p->ApplyVelocityToPos(pos);

	// Update run particle system.
	Vec3 emitDirection = Vec3(cosf(-rot.Y + 3.14f / 2), 0, sinf(-rot.Y + 3.14f / 2)).Normalize();
	float particleAcceleration = 10.0f;

	bool running = p->GetStates()->isMoving && !this->isInAir;

	this->mParticleSystemRun->SetEmitFrequency(running? RUN_PARTICLE_EMIT_RATE : FLT_MAX);
	this->mParticleSystemRun->SetEmitPos(XMFLOAT3(pos.X, pos.Y, pos.Z));
	this->mParticleSystemRun->SetEmitDir(XMFLOAT3(emitDirection.X, emitDirection.Y, emitDirection.Z));
	this->mParticleSystemRun->SetConstantAccel(XMFLOAT3(emitDirection.X * particleAcceleration, emitDirection.Y * particleAcceleration, emitDirection.Z * particleAcceleration));

	// Update dizzy particle system.
	this->mParticleSystemDizzy->SetActive(isDizzy);

	if (isDizzy)
	{
		mParticleSystemDizzyAngle += DegreesToRadians(135.0f) * deltaTime;

		if (mParticleSystemDizzyAngle > XM_2PI)
			mParticleSystemDizzyAngle -= XM_2PI;

		Vec3 headPosition = pos + Vec3(0.0f, 10.0f, 0.0f);
		Vec3 particleSystemPosition = headPosition + Vec3(cosf(mParticleSystemDizzyAngle), 0.0f, sinf(mParticleSystemDizzyAngle)) * 3.0f;

		this->mParticleSystemDizzy->SetEmitPos(XMFLOAT3(particleSystemPosition.X, particleSystemPosition.Y, particleSystemPosition.Z));
	}

	getOwner()->updatePos(pos);

#ifdef DEBUG
	if (getOwner()->fId == 1)
	{
		if (getOwner()->getModules()->input->isKeyDown('E'))
		{
			Vec3 newVel = p->GetVelocity() + Vec3(0.0f, 300.0f*deltaTime, 0.0f);
			if (newVel.Y < 150.0f)
			{
				p->SetVelocity(newVel);
				p->GetStates()->isJumping = true;
				getOwner()->updatePos(pos);
			}
		}

		if (getOwner()->getModules()->input->isKeyDown(VK_SHIFT))
		{
			speed = 150.0f;
		}
		else if (speed == 150.0f)
		{
			speed = 50.0f;
		}
	}
#endif
}

Vec3 Movement::GetLook()
{
	return this->camLook;
}

void Movement::setCamera(Vec3 look, Vec3 right, Vec3 up)
{
	//if (p)
	{
		Vec3 lookY = look;
		lookY.Y = 0;
		lookY.Normalize();

		yaw = asinf(lookY.X);
		if (lookY.Z > 0)
			yaw = -yaw - 3.14f/2;
		else
			yaw = yaw + 3.14f - 3.14f / 2;

		float pitch = -asinf(look.Y);

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
	if ((!getOwner()->ground && timeFalling > 0.3f) || !canJump)
	{
		return;
	}
	else if (canMove)
	{
		Vec3 pos = getOwner()->returnPos();

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

			getOwner()->updatePos(pos);

			Entity *owner = getOwner();
			GravityComponent *gravity = owner->getComponent<GravityComponent*>("Gravity");

			if (gravity)
			{
				gravity->setEnabled(false);
				this->timeUntilGravityEnable = MAX_JUMP_KEY_TIME;
			}

			owner->getModules()->sound->PlaySound(GetPlayerSoundFile("player/jump1.wav"), 0.6f, &pos.X);

			if (getOwner()->IsAnimated())
			{
				if (getOwnerId() == 1)
				{
					// Play jump animation for player.
					getOwner()->SetAnimation(1, false);
				}
				else if (getOwner()->getComponent<AI *>("AI"))
				{
					// Play jump animation for AI.
					//getOwner()->SetAnimation(5, false);
				}
			}
		}
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

void Movement::dizzyTimer(float deltaTime)
{
	this->dizzyCounter += deltaTime;
	if (this->dizzyCounter >= this->dizzyMaxTimer)
	{
		this->canMove = true;
		this->isDizzy = false;
		getOwner()->sendMessage("notDizzy", this);
	}
}

void Movement::setIsDizzy(Message const &msg)
{
	if (!isDizzy)
	{
		this->isDizzy = true;
		this->dizzyCounter = 0;
		this->canMove = false;
		Vec3 position = getOwner()->returnPos();
		getOwner()->getModules()->sound->PlaySound("birds.wav", 1.0f, &position.X);
	}

}

bool Movement::getIsInAir()
{
	return this->isInAir;
}

float Movement::getTimeFalling()
{
	return this->timeFalling;
}