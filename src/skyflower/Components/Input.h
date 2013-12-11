#ifndef SKYFLOWER_INPUT_H
#define SKYFLOWER_INPUT_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

class Input : public Component {

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

		requestMessage("W", &Input::W);
		requestMessage("S", &Input::S);
		requestMessage("A", &Input::A);
		requestMessage("D", &Input::D);
		requestMessage("E", &Input::E);
	}
private:

	void W(const Message& message)
	{
		cout << "Pressed W\n";
		sendMessage("MoveForward");
		sendMessage("ApplyGravity");
	}

	void S(const Message& message)
	{
		cout << "Pressed S\n";
		sendMessage("MoveBackward");
	}

	void A(const Message& message)
	{
		cout << "Pressed A\n";
		sendMessage("MoveLeft");
	}

	void D(const Message& message)
	{
		cout << "Pressed D\n";
		sendMessage("MoveRight");
	}

	void E(const Message& message)
	{
		cout << "Pressed E\n";
		sendMessage("Jump");
	}

};

#endif