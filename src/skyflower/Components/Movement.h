#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <string>

#include "shared/Vec3.h"
#include "physics/PhysicsEngine.h"
#include "graphics/ParticleSystem.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

#define MAX_JUMP_KEY_TIME 0.4f

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
	float GetSpeed();
	void SetSpeed(float speed);

	bool IsDizzy()
	{
		return isDizzy;
	}

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
	Vec3 camLook;
	float speed;
	bool canMove;
	float timeUntilGravityEnable;
	float yaw;
	float targetRot;
	float walkAngle;
	float timeFalling;
	JumpDirection mInitialJumpDir;
	float dizzyMaxTimer;
	float dizzyCounter;
	bool isDizzy;
	Vec3 pRot;

	ParticleSystem *mParticleSystemRun;
	ParticleSystem *mParticleSystemDizzy;
	float mParticleSystemDizzyAngle;

	void DoJumpStuff(float &speed);
};

#endif
