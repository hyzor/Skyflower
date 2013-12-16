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
			sendMessageToEntity(this->getOwnerId(), "StartMoveForward");
			break;
		case 'S':
			sendMessageToEntity(this->getOwnerId(), "StartMoveBackward");
			break;
		case 'A':
			sendMessageToEntity(this->getOwnerId(), "StartMoveLeft");
			break;
		case 'D':
			sendMessageToEntity(this->getOwnerId(), "StartMoveRight");
			break;
		case VK_SPACE:
			sendMessageToEntity(this->getOwnerId(), "Jump");
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
			//sendMessage("StopMoveForward");
			sendMessageToEntity(this->getOwnerId(), "StopMoveForward");
			break;
		case 'S':
			sendMessageToEntity(this->getOwnerId(), "StopMoveBackward");
			break;
		case 'A':
			sendMessageToEntity(this->getOwnerId(), "StopMoveLeft");
			break;
		case 'D':
			sendMessageToEntity(this->getOwnerId(), "StopMoveRight");
			break;
		default:
			break;
		}
	}
};

#endif