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

	HelpText(string text, float range) : Component("HelpText")
	{ 
		m_text = text;
		m_range = range;
		m_menuActive = false;
		m_active = true;
		first = true;
		m_textToPrint = "";
		m_timer = 0.0f;
	};
	virtual ~HelpText() {};
	void addedToEntity();
	void update(float dt);
	void removeFromEntity();
	

private:
	string m_text;
	string m_textToPrint;
	float m_timer;
	float m_range;
	float top; // top left corner
	bool m_menuActive;
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

	vector<string> SplitString(string str, char token);

	void activate(Message const& msg);
	void deActivate(Message const& msg);
	void hide(Message const& msg);

	void menu_activate(Message const& msg);
	void menu_deActivate(Message const& msg);
};

#endif