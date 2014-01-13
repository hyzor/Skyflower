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
	};
	virtual ~Button() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A AI was added to the system." << endl;

		requestMessage("Ground", &Button::Activate);
		moveTo = getEntityPos();
		startPos = moveTo;
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

	void update(float dt)
	{

		if (activated > 0)
		{
			activated -= dt;
			if (activated <= 0)
				Deactivate();
		}

		updateEntityPos(getEntityPos() + (moveTo - getEntityPos())/20);
	}
private:

	void Activate(Message const& msg);

	void Deactivate();

	float activated;

	Vec3 moveTo;
	Vec3 startPos;



};

#endif