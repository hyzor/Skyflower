#ifndef BUTTON_H
#define BUTTON_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Collision.h"
#include "PotentialField.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Button : public Component {

public:

	// constructor - age is fixed at creation time
	Button() : Component("Button")
	{
		activated = 0;
		act = false;
	};
	virtual ~Button() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A AI was added to the system." << endl;

		requestMessage("Ground", &Button::Activate);
		moveTo = getEntityPos();
		startPos = moveTo;
		downPos = Vec3(0, -getEntityScale().Y, 0);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	void update(float dt);
private:

	void Activate(Message const& msg);

	void Deactivate();

	float activated;
	bool act;

	Vec3 moveTo;
	Vec3 startPos;
	Vec3 downPos;



};

#endif