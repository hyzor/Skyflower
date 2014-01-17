#ifndef MENU_H
#define MENU_H
#include "graphics\GraphicsEngine.h"
#include <string>
#include <vector>

using namespace std;

class Menu
{
public:
	struct Button
	{
		bool _active;
		string _tex;
		string _hoverTex;
		Vec3 _position;
		Button() : _active(false), _tex(""), _position(Vec3()){}
		Button(bool active, string tex, string hoverTex, Vec3 pos)
			: _active(active), _tex(tex), _hoverTex(hoverTex), _position(pos){}
	};

public:
	Menu();
	void init(GraphicsEngine *g);
	virtual ~Menu();

	// Set wether or not the menu is active
	void setState(bool show);

	// Is the menu active? I don´t know bro. Better ask that mo-fo 
	bool isActive();

	// Draw the menu
	void draw(GraphicsEngine *g);

	void buttonPressed(unsigned short key);
private:
	bool m_active;
	int selectedButton;
	string m_bg;
	std::vector<Button> m_buttons;

};

#endif