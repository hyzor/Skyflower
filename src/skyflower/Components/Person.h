#ifndef PERSON_H
#define PERSON_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

class Person : public Component {

public:

	// constructor - age is fixed at creation time
	Person(string name, int age) : Component("Person"), fName(name), fAge(age) {
	};
	virtual ~Person() {};

	// get the age of the person
	int getAge() { return fAge; }



	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << fName << " with age " << fAge << " was added to the system." << endl;

		requestMessage("Hello", &Person::printHello);

		requestMessage("NextYear", &Person::nextYear);

	}

	void sendAMessage(string message)
	{
		cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}


private:

	int fAge;
	string fName;
	Vec3 pos;

	void printHello(Message const & msg) {
		cout << "Det är " << this->fName << " som skriver detta meddelandet!" << endl;
		pos = getEntityPos();
		cout << "x: " << this->pos.X << " y: " << this->pos.Y << " z: " << this->pos.Z << " " << endl;
		this->pos.X = 10;
		updateEntityPos(this->pos);
	}

	// we receive a next year message - update the age, and let everyone know that our age changed
	void nextYear(Message const & msg) {

		// first, we update our age
		++fAge;
		cout << fName << " had a birthday and is now " << fAge << " years old" << endl;

		// bob dies
		if (fAge == 66) destroyEntity(getOwnerId());

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
};

#endif