#ifndef MESSENGER_H
#define MESSENGER_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Messenger : public Component {

public:

	Messenger() : Component("Messenger"){};
	virtual ~Messenger() {};

	void addedToEntity()
	{
		cout << "Messenger lever!" << endl;

		requestMessage("All", &Messenger::printToAll);
	}

private:

	void printToAll(Message const & msg)
	{
		
	}
};

#endif