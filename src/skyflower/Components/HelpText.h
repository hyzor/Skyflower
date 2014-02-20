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
		m_alpha = 0.0f;
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
	float m_alpha;
	bool m_active;
	int bgID;
	int duckID;
	Vec3 orig;
	float xPos;
	UINT width, height;
	void activate(Message const& msg);
	void deActivate(Message const& msg);
};

#endif