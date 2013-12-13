#ifndef PLATFORM_H
#define PLATFORM_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Physics.h"
using namespace std;
using namespace Cistron;

class Platform : public Component {

public:

	// constructor - age is fixed at creation time
	Platform(float xPos, float yPos, float zPos, bool isMovingUpDown, bool isMovingFrontBack, bool isMovingSideways) : Component("Movement"), isMovingUpDown(isMovingUpDown), isMovingFrontBack(), isMovingSideways(isMovingSideways)
	{
		this->p = new Physics();
		this->startPos.X = xPos;
		this->startPos.Y = yPos;
		this->startPos.Z = zPos;
		this->maxMove = 10;

	};
	virtual ~Platform() { delete this->p; };

	Physics* getPhysicsInstance() { return this->p; };



	// we are added to an Entity, and thus to the component system
	void addedToEntity() {

		cout << "A platformcomponent was added to the system." << endl;

		requestMessage("update", &Platform::update);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:
	Physics* p;
	Vec3 startPos;
	float maxMove;
	bool isMovingUpDown;
	bool isMovingFrontBack;
	bool isMovingSideways;
	bool isMovingUp;
	bool isMovingDown;
	bool isMovingFront;
	bool isMovingBack;
	bool isMovingLeft;
	bool isMovingRight;


	void update(Message const& msg)
	{
		cout << "got update message!" << endl;
		if (this->isMovingUpDown)
		{
			Vec3 temp = this->getEntityPos();
			if (isMovingUp)
			{
				//if moving too far up
				if (temp.Y > this->startPos.Y + this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformDown");
					this->isMovingUp = false;
					this->isMovingDown = true;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformUp");
				}
			}
			else
			{
				//if moving too far down
				if (temp.Y < this->startPos.Y - this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformUp");
					this->isMovingUp = false;
					this->isMovingDown = true;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformDown");
				}
			}
		}
	}

};

#endif