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

	Button(bool toggle) : Component("Button")
	{
		activated = 0;
		act = false;
		first = true;
		this->toggle = toggle;
	};
	virtual ~Button() {};

	void addedToEntity()
	{
		requestMessage("Ground", &Button::Activate);
	}

	bool isDown();

	void update(float dt);
	void Deactivate();

private:

	float activated;

	bool act;
	bool first;
	bool toggle;
	bool down;

	Vec3 moveTo;
	Vec3 startPos;
	Vec3 downPos;

	void Activate(Message const& msg);



};

#endif