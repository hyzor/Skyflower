#include "Cistron.h"
#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

class Monster : public Component {

public:

	// name
	string fName;

	Monster(string name) : Component("Monster"), fName(name){};
	virtual ~Monster() {};

	void addedToEntity() {

		cout << " Monstret med namnet" << this->fName << "har kommit till liv!" << endl;

		requestMessage("Hello", &Monster::printHello);

	}

private:

	void printHello(Message const & msg) {
		cout << "Det är " << this->fName << " som skriver detta meddelandet!" << endl;
	}

};

#endif
