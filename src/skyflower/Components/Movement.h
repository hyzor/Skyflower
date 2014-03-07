#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <string>

#include "shared/Vec3.h"
#include "physics/PhysicsEngine.h"
#include "graphics/ParticleSystem.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

#define MAX_JUMP_KEY_TIME 0.2f

enum JumpDirection
{
	None,
	Forward,
	Backward,
	Left,
	Right
};

class Movement : public Component
{
public:
	Movement(float speed);
	virtual ~Movement();

	void addedToEntity();
	void removeFromEntity();
	void update(float deltaTime);
	void setCamera(Vec3 look, Vec3 right, Vec3 up);
	void setYaw(float yaw);
	void moveforward();
	void SetSpeed(float speed);

	float GetSpeed();
	float getTimeFalling();

	bool getIsInAir();

	Vec3 GetLook();

	bool getIsDizzy()
	{
		return this->isDizzy;
	}

	bool isMovingBackwards()
	{
		return this->isMovingBackward;
	}

	void CanMove(bool canMove)
	{
		this->canMove = canMove;
	}

	bool canJump;

private:
	void startMoveForward(Message const& msg);
	void startMoveBackward(Message const& msg);
	void startMoveLeft(Message const& msg);
	void startMoveRight(Message const& msg);
	void stopMoveForward(Message const& msg);
	void stopMoveBackward(Message const& msg);
	void stopMoveLeft(Message const& msg);
	void stopMoveRight(Message const& msg);
	void stopMoving(Message const& msg);
	void startMoving(Message const& msg);
	void inAir(Message const& msg);
	void notInAir(Message const& msg);
	void Jump(Message const& msg);
	void StopJump(Message const& msg);
	void setIsDizzy(Message const& msg);
	void dizzyTimer(float deltaTime);


private:
	PhysicsEntity* p;

	bool isMovingForward;
	bool isMovingBackward;
	bool isMovingLeft;
	bool isMovingRight;
	bool isInAir;
	bool canMove;
	bool isDizzy;

	Vec3 camLook;
	Vec3 pRot;

	float speed;
	float timeUntilGravityEnable;
	float yaw;
	float targetRot;
	float walkAngle;
	float timeFalling;
	float dizzyMaxTimer;
	float dizzyCounter;
	float respawnTimer;
	float turnAngle;
	float idleTimer;
	float mParticleSystemDizzyAngle;

	JumpDirection mInitialJumpDir;

	ParticleSystem *mParticleSystemRun;
	ParticleSystem *mParticleSystemDizzy;

	void DoJumpStuff(float &speed);
};

#endif
