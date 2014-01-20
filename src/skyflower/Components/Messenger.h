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
		//cout << "Messenger lever!" << endl;

		//requestMessage("All", &Messenger::printToAll);
		//requestMessage("update", &Messenger::update);
	}

private:

	void printToAll(Message const & msg)
	{
		
	}
	void update(Message const & msg)
	{
		sendMessageToEntity(this->getOwnerId(), "update");
	}
};

#endif