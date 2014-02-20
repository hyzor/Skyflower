#ifndef BUTTON_H
#define BUTTON_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Collision.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Button : public Component {

public:

	// constructor - age is fixed at creation time
	Button(bool toggle) : Component("Button")
	{
		activated = 0;
		act = false;
		first = true;
		this->toggle = toggle;
	};
	virtual ~Button() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		requestMessage("Ground", &Button::Activate);
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

	bool isDown();

	void update(float dt);
private:

	void Activate(Message const& msg);

	void Deactivate();

	float activated;
	bool act;

	Vec3 moveTo;
	Vec3 startPos;
	Vec3 downPos;
	bool first;
	
	bool toggle;
	bool down;


};

#endif