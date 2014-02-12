#ifndef TOUCH_H
#define TOUCH_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Touch : public Component {

public:

	Touch() : Component("Touch")
	{
		activated = 0;
		activator = nullptr;
	};
	virtual ~Touch() {};



	void addedToEntity() {

		
	}

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

	void update(float dt);

	Entity* activator;
private:

	void Activate();

	void Deactivate();

	float activated;

	void findActivator();

};

#endif