#ifndef MENU_H
#define MENU_H
#include "graphics\GraphicsEngine.h"
#include "Sound/SoundEngine.h"
#include <string>
#include <vector>
#include "MenuItems.h"

using namespace std;

class MenuPage
{
public:
	void setVisible(bool visible)
	{
		for (unsigned i = 0; i < buttons.size(); i++)
		{
			buttons[i]->setVisible(visible);
		}
		for (unsigned i = 0; i < checkboxes.size(); i++)
		{
			checkboxes[i]->setVisible(visible);
		}
		for (unsigned i = 0; i < sliders.size(); i++)
		{
			sliders[i]->setVisible(visible);
		}
	}

public:
	std::vector<MenuButton*> buttons;
	std::vector<CheckBox*> checkboxes;
	std::vector<Slider*> sliders;
};

class Menu
{
public:
	enum MenuStatus
	{
		none,
		resume,
		exit
	};
	enum MenuPages
	{
		MenuPageMain = 0,
		MenuPageSettings,
		MenuPageInstructions,
		MenuPageCredits,
		MenuPageStart,
		MenuPageCount
	};
	struct Settings
	{
		bool _showHelpTexts;
		bool _isFullscreen;
		bool _mouseInverted;
		float _soundVolume;
		float _mouseSense;
	};
public:
	Menu();
	void init(GUI *g, int screenWidth, int screeenHeight, SoundEngine *soundEngine);
	virtual ~Menu();
	
	// Set wether or not the menu is active
	 void setActive(bool active);

	// Is the menu active? I don´t know bro. Better ask that mo-fo 
	 bool isActive();

	 void setActivePage(int page);
	 void setFirst(bool status);

	// Draw the menu
	 void draw();

	 int getStatus();
	 int getActivePage();
	 int isFirst();

	 void keyPressed(unsigned short key);

	 void mousePressed(Vec3 position);

	 // Events
	 void buttonResumeClicked();
	 void buttonExitClicked();
	 Settings getSettings() const;
	 void onResize(unsigned int width, unsigned int height);
	 void onMouseMove(Vec3 mousePos);
	 void onMouseDown(Vec3 mousePos);

private:
	 bool m_active;
	 int selectedButton;
	 int m_bg;
	 int m_instructionsWidth;
	 int m_instructions;
	 int m_creditsWidth;
	 int m_credits;
	 int settingsBox;
	 MenuPage m_pages[MenuPageCount];
	 int m_activePage;
	 MenuStatus status;
	 GUI* guiPtr;
	 SoundEngine* soundEngine;
	 Settings settings;
	 bool first = true;
private:
	 void setVisible(bool visible);
	 int width, height;
	 float scaleX, scaleY;
};

#endif
