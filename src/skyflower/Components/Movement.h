#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Physics.h"
#include "physics/Collision.h"
#include "Entity.h"
#include "Health.h"
using namespace std;
using namespace Cistron;

class Movement : public Component {

public:

	// constructor - age is fixed at creation time
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

		requestMessage("Jump", &Movement::Jump);
	}

	void removeFromEntity()
	{
		this->p = NULL;
	}

	void update(float deltaTime)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityRot();
		p->update(deltaTime);
		
		Health *health = getOwner()->getComponent<Health*>("Health");

		if (health)
		{
			if (pos.Y < -100)
			{
				health->setHealth(0);
				float soundPosition[3] = { 0.0f, 0.0f, 0.0f };

				if (getOwnerId() == 0)
					getOwner()->getModules()->sound->PlaySound("player/wilhelm_scream.wav", soundPosition, 1.0f, true);
			}
			if (!health->isAlive())
			{
				sendMessageToEntity(this->getOwnerId(), "Respawn");
				p->setVelocity(Vec3(0, 0, 0));
				health->setHealth(100);
				return;
			}
		}

		if (canMove)
		{
			if (this->isMovingForward) {
				if (this->isMovingLeft) {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 0.0f - 45.0f);
				}
				else if (this->isMovingRight) {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 0.0f + 45.0f);
				}
				else {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 0.0f);
				}
			}
			else if (this->isMovingBackward) {
				if (this->isMovingLeft) {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, -90.0f + -45.0f);
				}
				else if (this->isMovingRight) {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 180.0f - 45.0f);
				}
				else {
					p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 179.0f);
				}
			}
			else if (this->isMovingLeft) {
				p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, -90.0f);
			}
			else if (this->isMovingRight) {
				p->moveRelativeVec3(pos, this->camLook, speed * deltaTime, rot, 90.0f);
			}

			if (isMovingBackward || isMovingForward || isMovingLeft || isMovingRight)
			{
				p->setIsMoving(true);
			}
			else
			{
				p->setIsMoving(false);
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

	void moveforward()
	{
		this->isMovingForward = true;
	}

	float GetSpeed()
	{
		return speed;
	}

private:
	Physics* p;
	bool isMovingForward;
	bool isMovingBackward;
	bool isMovingLeft;
	bool isMovingRight;
	Vec3 camLook;
	float speed;
	bool canMove;

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

	void Jump(Message const& msg)
	{
		Vec3 pos = getEntityPos();

		if (p->jump(pos))
		{
			Entity *owner = getOwner();

			if (owner)
				owner->getModules()->sound->PlaySound("player/jump1.wav", &pos.X, 1.0f, false);
		}

		updateEntityPos(pos);
	}
};

#endif
