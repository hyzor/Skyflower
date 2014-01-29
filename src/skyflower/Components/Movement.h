#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "shared/util.h"
#include "physics/PhysicsEngine.h"
#include "physics/Collision.h"
#include "Entity.h"
#include "Health.h"
#include "Gravity.h"
using namespace std;
using namespace Cistron;

#define MAX_JUMP_KEY_TIME 0.4f

class Movement : public Component {

public:

	Movement(float speed) : Component("Movement")
	{
		this->isMovingForward = false;
		this->isMovingBackward = false;
		this->isMovingLeft = false;
		this->isMovingRight = false;
		this->canMove = true;
		this->camLook = Vec3(0.0f, 0.0f, 0.0f);
		this->speed = speed;
	}
	virtual ~Movement()
	{
	}

	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
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

	void removeFromEntity()
	{
		this->p = NULL;
	}

	void update(float deltaTime)
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
					getOwner()->getModules()->sound->PlaySound("player/wilhelm_scream.wav", soundPosition, 0.25f, true);
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

		float walkAngle = 0.0f;

		if (canMove)
		{
			if (this->isMovingForward) {
				if (this->isMovingLeft) {
					walkAngle = -45.0f;
				}
				else if (this->isMovingRight) {
					walkAngle = 45.0f;
				}
				else {
					walkAngle = 0.0f;
				}
			}
			else if (this->isMovingBackward) {
				if (this->isMovingLeft) {
					walkAngle = -90.0f - 45.0f;
				}
				else if (this->isMovingRight) {
					walkAngle = 180.0f - 45.0f;
				}
				else {
					walkAngle = 180.0f;
				}
			}
			else if (this->isMovingLeft) {
				walkAngle = -90.0f;
			}
			else if (this->isMovingRight) {
				walkAngle = 90.0f;
			}

			if (isMovingBackward || isMovingForward || isMovingLeft || isMovingRight)
			{
				//p->setIsMoving(true);
				p->GetStates().SetIsMoving(true);
				p->MoveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, walkAngle);

				// If the player is moving, rotate it to match the camera's direction.
				if (getOwnerId() == 1)
				{
					rot = Vec3(0.0f, -this->yaw + DegreesToRadians(90.0f + walkAngle), 0.0f);
				}
			}
			else
			{
				p->GetStates().SetIsMoving(false);
			}
		}

		updateEntityPos(pos);
		updateEntityRot(rot);
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

	void setCamera(Vec3 look, Vec3 right, Vec3 up)
	{
		if (p)
		{
			this->camLook = look;
		}
	}

	void setYaw(float yaw)
	{
		this->yaw = yaw;
	}

	void moveforward()
	{
		this->isMovingForward = true;
	}

	float GetSpeed()
	{
		return speed;
	}

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

	void startMoveForward(Message const& msg)
	{
		this->isMovingForward = true;
	}
	void startMoveBackward(Message const& msg)
	{
		this->isMovingBackward = true;
	}
	void startMoveLeft(Message const& msg)
	{
		this->isMovingLeft = true;
	}
	void startMoveRight(Message const& msg)
	{
		this->isMovingRight = true;
	}
	void stopMoveForward(Message const& msg)
	{
		this->isMovingForward = false;
	}
	void stopMoveBackward(Message const& msg)
	{
		this->isMovingBackward = false;
	}
	void stopMoveLeft(Message const& msg)
	{
		this->isMovingLeft = false;
	}
	void stopMoveRight(Message const& msg)
	{
		this->isMovingRight = false;
	}

	void stopMoving(Message const& msg)
	{
		this->canMove = false;
	}

	void startMoving(Message const& msg)
	{
		this->canMove = true;
	}

	void inAir(Message const& msg)
	{
		this->isInAir = true;
	}

	void notInAir(Message const& msg)
	{
		this->isInAir = false;
	}

	void Jump(Message const& msg)
	{
		if (!getOwner()->ground)
			return;

		Vec3 pos = getEntityPos();

		if (/*p->jump(pos)*/true)
		{
			Vec3 target = Vec3(36.0f, 20.0f, 39.5f);
			std::cout << "X: ";
			std::cout << pos.X;
			std::cout << " Y: " ;
			std::cout << pos.Y;
			std::cout << " Z: ";
			std::cout << pos.Z;
			std::cout << "\n";
			p->FireProjectileAt(pos, target);
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

	void StopJump(Message const& msg)
	{
		this->timeUntilGravityEnable = 0.0f;
	}
};

#endif
