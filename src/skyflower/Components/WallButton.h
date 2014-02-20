#ifndef WALLBUTTON_H
#define WALLBUTTON_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Collision.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class WallButton : public Component {

public:

	// constructor - age is fixed at creation time
	WallButton(Vec3 dir) : Component("WallButton")
	{
		activated = 0;
		act = false;
		first = true;
		this->dir = dir;
	};
	virtual ~WallButton() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		requestMessage("Wall", &WallButton::Activate);
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

	Vec3 dir;


};

#endif