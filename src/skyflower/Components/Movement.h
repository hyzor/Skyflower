#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Physics.h"
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
		//cout << "hej det �r jag som ropar p� denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:
	Physics* p;

	void applyGravity(Message const& msg)
	{
		//this->p->addGravityCalc(this->pos);
	}

	void moveForward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveForward(pos);

		updateEntityPos(pos);
	}

	void moveBackward(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveBackward(pos);

		updateEntityPos(pos);
	}

	void moveLeft(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveLeft(pos);

		updateEntityPos(pos);
	}

	void moveRight(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		p->moveRight(pos);

		updateEntityPos(pos);
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
		cout << "Ska r�ra mig upp!" << endl;
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