#ifndef HELPTEXT_H
#define HELPTEXT_H

#include "Cistron.h"
#include "Entity.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;
using namespace Cistron;

class HelpText : public Component {

public:

	// constructor - age is fixed at creation time
	HelpText(string text, float range) : Component("HelpText")
	{ 
		m_text = text;
		m_range = range;
		m_active = true;
		//m_showFactor = 0.0f;
		first = true;

	};
	virtual ~HelpText() {};
	void addedToEntity();
	void update(float dt);
	// we are added to an Entity, and thus to the component system

	void removeFromEntity()
	{
		Entity *owner = getOwner();
		assert(owner);	
	}

private:
	string m_text;
	float m_range;
	//float m_showFactor;
	float top; // top right corner
	bool m_active;
	bool first;

	// Texture ID's
	int bgID;
	int duckID;

	// Original position of the object
	Vec3 orig;

	// x-position of the text
	float xPos;

	//screen width and heigh
	UINT width, height;

	void activate(Message const& msg);
	void deActivate(Message const& msg);
};

#endif