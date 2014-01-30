#ifndef MENU_H
#define MENU_H
#include "graphics\GraphicsEngine.h"
#include <string>
#include <vector>
#include "MenuItems.h"

using namespace std;

class Menu
{
public:
	enum MenuStatus
	{
		none,
		resume,
		exit
	};
	/*struct Button
	{
		bool _active;
		string _tex;
		string _hoverTex;
		Vec3 _position;
		Button() : _active(false), _tex(""), _position(Vec3()){}
		Button(bool active, string tex, string hoverTex, Vec3 pos)
			: _active(active), _tex(tex), _hoverTex(hoverTex), _position(pos){}
	}; */

public:
	Menu();
	void init(GUI *g, int screenWidth, int screeenHeight);
	virtual ~Menu();
	
	// Set wether or not the menu is active
	 void setActive(bool active);

	// Is the menu active? I don´t know bro. Better ask that mo-fo 
	 bool isActive();

	// Draw the menu
	 void draw();

	 int getStatus();

	 void keyPressed(unsigned short key);

	 void mousePressed(Vec3 position);

	 // Events
	 void buttonResumeClicked();
	 void buttonExitClicked();
	 bool isFullscreen();
	 void onResize(unsigned int width, unsigned int height);
	 void onMouseMove(Vec3 mousePos);

private:
	 bool m_active;
	 int selectedButton;
	 int m_bg;
	 int settingsBox;
	 std::vector<MenuButton*> m_buttons;
	 std::vector<CheckBox*> m_checkboxes;
	 MenuStatus status;
	 GUI* guiPtr;
	 bool fullscreen;
private:
	 void setVisible(bool visible);
	 int width, height;
	 float oldScaleX, oldScaleY;

	
	
};

#endif