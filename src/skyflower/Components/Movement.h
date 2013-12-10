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
	Movement(float x, float y, float z) : Component("Movement")
	{ 
		pos.X = x;
		pos.Y = y;
		pos.Z = z;
	};
	virtual ~Movement() { delete this->p; };

	Physics* getPhysicsInstance() { return this->p; };



	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		std::string x, y, z;
		std::ostringstream ss;

		ss << pos.X;
		x = ss.str();
		ss.str("");

		ss << pos.Y;
		y = ss.str();
		ss.str("");

		ss << pos.Z;
		z = ss.str();
		ss.str("");

		cout << "A movementcomponent with position (" << x + ", " + y + ", " + z + ") was added to the system." << endl;

		//requestMessage("Hello", &Movement::printHello);

		//requestMessage("NextYear", &Movement::nextYear);

	
		requestMessage("CheckCollision", &Movement::applyGravity);
		requestMessage("CheckInput", &Movement::inputMovement);

	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:
	Vec3 pos;
	Physics* p;

	void printHello(Message const & msg) 
	{
		//cout << "Det är " << this->fName << " som skriver detta meddelandet!" << endl;
	}

	// we receive a next year message - update the age, and let everyone know that our age changed
	void nextYear(Message const & msg) 
	{

		// first, we update our age
		//++fAge;
		//cout << fName << " had a birthday and is now " << fAge << " years old" << endl;

		// bob dies
		//if (fAge == 66) destroyEntity(getOwnerId());

		/**
		* Send a message to everyone that we aged.
		* There are many different functions available to make it easy to send messages.
		* Basically there are three ways to send a message:
		*    - sendLocalMessage: send a message to all the components in THIS Entity who have requested this message type.
		- sendMessage: send a message to all the components in the system who have requested this message type.
		- sendMessageToEntity: send a message to a particular Entity (defined by its id), which can differ from this one.
		*
		* Next to these three methods, there are several overloaded versions of each,
		* based on whether you want to give a payload, etc. The different basic versions are:
		*
		* sendLocalMessage("MessageName"); // payload is not provided
		* sendLocalMessage("MessageName", payloadPointer); // payload provided
		*
		* sendMessage("MessageName"); // payload is not provided
		* sendMessage("MessageName", payloadPointer); // payload provided
		*
		* sendMessageToEntity(EntityId, "MessageName"); // payload is not provided
		* sendMessageToEntity(EntityId, "MessageName", payloadPointer); // payload provided
		* EntityId is obtained by calling getOwnerId() on a component.
		*
		* In this case, we just want to announce the whole world of our birthday.
		*/
		sendMessage("Birthday");
	}

	void applyGravity(Message const& msg)
	{
		this->p->addGravityCalc(this->pos);
	}

	void inputMovement(Message const& msg)
	{
		//checka input
		
		//w
		this->p->moveForward(this->pos);

		//s
		this->p->moveBackward(this->pos);

		//a
		this->p->moveLeft(this->pos);

		//d
		this->p->moveRight(this->pos);

		//space
		this->p->jump(this->pos);
	}


};

#endif