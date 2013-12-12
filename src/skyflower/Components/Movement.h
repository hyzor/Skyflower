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
using namespace std;
using namespace Cistron;

class Movement : public Component {

public:

	// constructor - age is fixed at creation time
	Movement() : Component("Movement")
	{ 
		this->p = new Physics();
	};
	virtual ~Movement() { delete this->p; };

	Physics* getPhysicsInstance() { return this->p; };



	// we are added to an Entity, and thus to the component system
	void addedToEntity() {

		cout << "A movementcomponent was added to the system." << endl;

		//requestMessage("Hello", &Movement::printHello);

		//requestMessage("NextYear", &Movement::nextYear);

	
		requestMessage("CheckCollision", &Movement::applyGravity);
		//requestMessage("CheckInput", &Movement::inputMovement);
		requestMessage("MoveForward", &Movement::moveForward);
		requestMessage("MoveBackward", &Movement::moveBackward);
		requestMessage("MoveLeft", &Movement::moveLeft);
		requestMessage("MoveRight", &Movement::moveRight);
		requestMessage("Jump", &Movement::Jump);
		requestMessage("Update", &Movement::update);
		requestMessage("moveUpDown", &Movement::moveUpDown);
		requestMessage("moveFrontBack", &Movement::moveFrontBack);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:
	Physics* p;

	void applyGravity(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->update(0.001f);
		this->p->addGravityCalc(pos);

		//pos.Y -= 0.01f;

		std::vector<CollisionInstance*> instances = Collision::GetInstance()->GetCollisionInstances();
		Ray r = Ray(pos, Vec3(0, -65, 0));
		float col = 0;
		for (size_t i = 0; i < instances.size(); i++)
		{
			if (instances[i] != getEntityCollision())
			{
				float t = instances[i]->Test(r);
				if (t > 0)
				{
					col = t;
					break;
				}
			}
		}
		if (col) //om kollision flytta tillbaka
		{
			pos.Y -= (1 - col)*-65;
			p->setVelocity(Vec3());
			p->setJumping(false);
		}

		updateEntityPos(pos);
	}

	void moveForward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityRot();

		if (p->toDegrees(rot.Y) != -90.0f)
		{
			p->resetRot(rot);
			p->rotateY(rot, -90.0f);
		}
		p->moveForward(pos);
		updateEntityPos(pos);
		updateEntityRot(rot);

	}

	void moveBackward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityRot();
		
		if (p->toDegrees(rot.Y) != 90.0f)
		{
			p->resetRot(rot);
			p->rotateY(rot, 90.0f);
		}
		p->moveBackward(pos);
		updateEntityPos(pos);
		updateEntityRot(rot);
	}

	void moveLeft(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityRot();

		if (p->toDegrees(rot.Y) != 180.0f)
		{
			p->resetRot(rot);
			p->rotateY(rot, 180.0f);
		}
		p->moveLeft(pos);

		updateEntityPos(pos);
		updateEntityRot(rot);
	}

	void moveRight(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityPos();
		//if (p->toDegrees(rot.Y) != 0.0f)
		//{
			p->resetRot(rot);
		//}
		p->moveRight(pos);

		updateEntityPos(pos);
		updateEntityRot(rot);
	}

	void Jump(Message const& msg)
	{
		Vec3 pos = getEntityPos();

		if (p->jump(pos))
		{
			//float soundPosition[3] = {0.0f, 0.0f, 0.0f};
			//getOwner()->getModules()->sound->PlaySound("jump.wav", soundPosition, 1.0f, true); 
		}

		updateEntityPos(pos);
	}

	void update(Message const& msg)
	{
		applyGravity(msg);
	}

	void moveUpDown(Message const& msg)
	{
		cout << "Ska rï¿½ra mig upp!" << endl;
		Vec3 pos = getEntityPos();
		p->moveUp(pos);

		updateEntityPos(pos);
	}

	void moveFrontBack(Message const& msg)
	{
		Vec3 pos = getEntityPos();

	}


};

#endif