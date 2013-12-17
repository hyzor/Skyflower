#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include "ScriptHandler.h"
using namespace std;
using namespace Cistron;


class LUAScript : public Component {

public:

	LUAScript(string script) : Component("LUAScript")
	{
		ScriptHandler::GetInstance()->Run("script");
	};
	virtual ~LUAScript() {};

	void addedToEntity()
	{
		
	}

private:

	void printToAll(Message const & msg)
	{
		
	}
	void update(Message const & msg)
	{
		sendMessageToEntity(this->getOwnerId(), "update");
	}
	void write(string str)
	{
		cout << "YIAOEYAIOEUY OK? \n";
	}
};


#endif