#ifndef MESSENGER_H
#define MESSENGER_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;


class Messenger : public Component {

public:

	// name
	string fName;

	Messenger(string name) : Component("Messenger"), fName(name){};
	virtual ~Messenger() {};

	void addedToEntity()
	{
		cout << " Monstret med namnet" << this->fName << "har kommit till liv!" << endl;

		requestMessage("All", &Messenger::printToAll);
	}

private:

	void printToAll(Message const & msg)
	{
		cout << "Det är " << this->fName << " som skriver detta meddelandet!" << endl;
	}
};

#endif