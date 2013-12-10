#ifndef RENDER_H
#define RENDER_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include "shared/Vec3.h"
#include "graphics/GraphicsEngine.h"
using namespace std;
using namespace Cistron;

class Render : public Component {

public:

	// constructor - age is fixed at creation time
	Render() : Component("Render")
	{ 

	};
	virtual ~Render() {};

	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{


		cout << "A rendercomponent was added to the system and to the graphicsengine." << endl;

		//requestMessage("Draw", &Render::Draw); 
		
		//requestMessage("Hello", &Movement::printHello);

		//requestMessage("NextYear", &Movement::nextYear);
		requestMessage("setModel", &Render::setModel);
		requestMessage("setModelPos",&Render::setModelPos);
		requestMessage("setModelRot", &Render::setModelRot);
		requestMessage("setModelScale", &Render::setModelScale);
		requestMessage("setModelVisible", &Render::setModelVisible);

	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:
	bool isVisible;

	void printHello(Message const & msg) 
	{
		//cout << "Det är " << this->fName << " som skriver detta meddelandet!" << endl;
	}

	// we receive a next year message - update the age, and let everyone know that our age changed
	void nextYear(Message const & msg) 
	{
		sendMessage("Birthday");
	}

	void setModel(Message const & msg)
	{
		
	}

	void setModelPos(Message const & msg)
	{
		Vec3 pos = getEntityPos();
	}
	void setModelRot(Message const & msg)
	{

	}
	void setModelScale(Message const & msg)
	{

	}
	void setModelVisible(Message const & msg)
	{

	}
};

#endif