#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Physics.h"
#include "physics/Collision.h"
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
		//this->p->addGravityCalc(this->pos);
		Vec3 pos = getEntityPos();

		pos.Y -= 10.0f;

		std::vector<CollisionInstance*> instances = Collision::GetInstance()->GetCollisionInstances();
		Ray r = Ray(pos, Vec3(0, -65, 0));
		float col = 0;
		for (int i = 0; i < instances.size(); i++)
		{
			float t = instances[i]->Test(r);
			if (t > 0)
			{
				col = t;
				break;
			}
		}
		if(col) //om kollision flytta tillbaka
			pos.Y -= (1 - col)*-65;

		updateEntityPos(pos);
	}

	void moveForward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveForward(pos);

		updateEntityPos(pos);
		applyGravity(msg);

	}

	void moveBackward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveBackward(pos);

		updateEntityPos(pos);
		applyGravity(msg);
	}

	void moveLeft(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveLeft(pos);

		updateEntityPos(pos);
		applyGravity(msg);
	}

	void moveRight(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveRight(pos);

		updateEntityPos(pos);
		applyGravity(msg);
	}

	void Jump(Message const& msg)
	{
		//Vec3 pos = getEntityPos();
		//p->
		//p->jump(pos);

		//updateEntityPos(pos);
	}

	void update(Message const& msg)
	{
		
	}

	void moveUpDown(Message const& msg)
	{
		cout << "Ska röra mig upp!" << endl;
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