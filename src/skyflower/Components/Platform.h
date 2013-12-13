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
	Platform(float xPos, float yPos, float zPos, bool isMovingUpDown, bool isMovingFrontBack, bool isMovingSideways) : Component("Movement"), isMovingUpDown(isMovingUpDown), isMovingFrontBack(isMovingFrontBack), isMovingSideways(isMovingSideways)
	{
		this->p = new Physics();
		this->startPos.X = xPos;
		this->startPos.Y = yPos;
		this->startPos.Z = zPos;
		this->isMovingUp = true;
		this->isMovingDown = false;
		this->isMovingFront = true;
		this->isMovingBack = false;
		this->isMovingLeft = true;
		this->isMovingRight = false;
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
			else if (isMovingDown)
			{
				//if moving too far down
				if (temp.Y < this->startPos.Y - this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformUp");
					this->isMovingUp = true;
					this->isMovingDown = false;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformDown");
				}
			}
		}
		if (this->isMovingFrontBack)
		{
			Vec3 temp = this->getEntityPos();
			if (isMovingFront)
			{
				//if moving too much forward
				if (temp.X > this->startPos.X + this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformBack");
					this->isMovingFront = false;
					this->isMovingBack = true;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformFront");
				}
			}
			else if (isMovingBack)
			{
				//if moving too far down
				if (temp.X < this->startPos.X - this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformFront");
					this->isMovingFront = true;
					this->isMovingBack = false;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformBack");
				}
			}
		}
		if (this->isMovingSideways)
		{
			Vec3 temp = this->getEntityPos();
			if (isMovingRight)
			{
				//if moving too much forward
				if (temp.Z > this->startPos.Z + this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformLeft");
					this->isMovingRight = false;
					this->isMovingLeft = true;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformRight");
				}
			}
			else if (isMovingLeft)
			{
				//if moving too far down
				if (temp.Z < this->startPos.Z - this->maxMove)
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformRight");
					this->isMovingRight = true;
					this->isMovingLeft = false;
				}
				else
				{
					sendMessageToEntity(this->getOwnerId(), "movePlatformLeft");
				}
			}
		}
	}

};

#endif