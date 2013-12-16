#ifndef SKYFLOWER_INPUT_H
#define SKYFLOWER_INPUT_H

#include "Cistron.h"
#include "Entity.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;
using namespace Cistron;

class Input : public Component, InputListener {

public:

	// constructor - age is fixed at creation time
	Input() : Component("Input")
	{ 

	};
	virtual ~Input() {};


	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		cout << "An inputcomponent was added to the system." << endl;

		Entity *owner = getOwner();
		assert(owner);
		owner->getModules()->input->AddListener(this);
	}

	void removeFromEntity()
	{
		Entity *owner = getOwner();
		assert(owner);
		owner->getModules()->input->RemoveListener(this);
	}

	void OnKeyDown(unsigned short key)
	{
		switch (key)
		{
		case 'W':
			sendMessage("StartMoveForward");
			break;
		case 'S':
			sendMessage("StartMoveBackward");
			break;
		case 'A':
			sendMessage("StartMoveLeft");
			break;
		case 'D':
			sendMessage("StartMoveRight");
			break;
		case VK_SPACE:
			sendMessage("Jump");
			break;
		default:
			break;
		}
	}

	void OnKeyUp(unsigned short key)
	{
		switch (key)
		{
		case 'W':
			sendMessage("StopMoveForward");
			break;
		case 'S':
			sendMessage("StopMoveBackward");
			break;
		case 'A':
			sendMessage("StopMoveLeft");
			break;
		case 'D':
			sendMessage("StopMoveRight");
			break;
		default:
			break;
		}
	}
};

#endif