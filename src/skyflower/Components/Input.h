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
			W();
			break;
		case 'S':
			S();
			break;
		case 'A':
			A();
			break;
		case 'D':
			D();
			break;
		case VK_SPACE:
			Space();
			break;
		}
	}

	void OnKeyUp(unsigned short key)
	{
	}

private:

	void W()
	{
		cout << "Pressed W\n";
		sendMessage("MoveForward");
		sendMessage("ApplyGravity");
	}

	void S()
	{
		cout << "Pressed S\n";
		sendMessage("MoveBackward");
	}

	void A()
	{
		cout << "Pressed A\n";
		sendMessage("MoveLeft");
	}

	void D()
	{
		cout << "Pressed D\n";
		sendMessage("MoveRight");
	}

	void Space()
	{
		cout << "Pressed Space\n";
		sendMessage("Jump");
	}

};

#endif