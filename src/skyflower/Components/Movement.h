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
		


		if (pos.Y < -100)
		{
			sendMessageToEntity(this->getOwnerId(), "Respawn");
			p->setVelocity(Vec3(0, 0, 0));
			return;
		}
		// CRACH!! FIX PLZ
		/*if (pos.Y < -100)
		{
			getOwner()->getComponent<Health*>("Health")->setHealth(0);

			float soundPosition[3] = {0.0f, 0.0f, 0.0f};
			getOwner()->getModules()->sound->PlaySound("player/wilhelm_scream.wav", soundPosition, 1.0f, true); 
		}
		if (!getOwner()->getComponent<Health*>("Health")->isAlive())
		{
			p->setVelocity(Vec3(0, 0, 0));
			pos = Vec3(0, 12, 0);
			getOwner()->getComponent<Health*>("Health")->setHealth(100);
		}*/
		//

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

		

		updateEntityPos(pos);
		updateEntityRot(rot);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
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


private:
	Physics* p;
	bool isMovingForward;
	bool isMovingBackward;
	bool isMovingLeft;
	bool isMovingRight;
	Vec3 camLook;
	float speed;

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