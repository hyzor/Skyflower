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
	Platform(bool isMovingUpDown, bool isMovingFrontBack) : Component("Movement"), isMovingUpDown(isMovingUpDown), isMovingFrontBack(isMovingFrontBack)
	{
		this->p = new Physics();
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
	bool isMovingUpDown;
	bool isMovingFrontBack;

	void update(Message const& msg)
	{
		cout << "got update message!" << endl;
		if (this->isMovingUpDown)
		{
			cout << "is moving up!" << endl;
			sendMessageToEntity(this->getOwnerId(), "moveUpDown");
		}
		else if (this->isMovingFrontBack)
		{
			cout << "is moving down!" << endl;
			sendMessageToEntity(this->getOwnerId(), "moveFrontBack");
		}
	}

};

#endif