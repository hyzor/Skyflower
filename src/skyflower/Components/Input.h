#ifndef INPUT_H
#define INPUT_H

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

		//requestMessage("Hello", &Movement::printHello);

		//requestMessage("NextYear", &Movement::nextYear);

	}
private:



};

#endif