#ifndef MENUITEMS_H
#define MENUITEMS_H
#include "graphics\GraphicsEngine.h"
#include <string>
#include <vector>
#include "GUI.h"
#include "GUIElement.h"

#define DEFAULT_SCREEN_WIDTH 1024
#define DEFAULT_SCREEN_HEIGHT 768

#define CHECKBOX_TEXTURE_HEIGHT 20
#define CHECKBOX_TEXTURE_WIDTH 20

#define SLIDER_BG_TEXTURE_WIDTH 150
#define SLIDER_BG_TEXTURE_HEIGHT 40
#define SLIDER_TEXTURE_WIDTH 20
#define SLIDER_TEXTURE_HEIGHT 50

#define BUTTON_TEXTURE_WIDTH 174
#define BUTTON_TEXTURE_HEIGHT 80

using namespace std;

class MenuItem
{
public:
	struct Rectangle
	{
		Vec3 _position;
		int _width;
		int _height;
		
		Rectangle(){};
		Rectangle(Vec3 pos, int width, int height)
			: _position(pos), _width(width), _height(height){}

		bool _isInside(Vec3 point)
		{
			if ((point.X > _position.X) && (point.X < _position.X + _width))
			{
				if (point.Y > _position.Y && point.Y < _position.Y + _height)
				{
					return true;
				}
			}
			return false;
		}
	};
	MenuItem(){};
	MenuItem(GUI *g, Vec3 position, int width, int height)
	{
		this->guiPtr = g;
		this->position = position;
		this->width = width;
		this->height = height;
		this->bounds = Rectangle(position, width, height);
		this->originalWidth = width;
		this->originalHeight = height;
		this->origPos = position;
	}
	virtual ~MenuItem(){};
	virtual void updateScreenRes(unsigned int x, unsigned int y);
	virtual vector<int> getTextureIDs(){ return this->textureIDs; }
	virtual void SetPosition(Vec3 pos);
	virtual void setScale(float x, float y);
	virtual Vec3 getPosition() { return this->position; }

protected:
	Vec3 position;
	Rectangle bounds;
	int width, height;
	GUI *guiPtr;
	int originalWidth, originalHeight;
	Vec3 origPos;
	vector<int> textureIDs;
};

class MenuButton : public MenuItem
{
public:
	
	MenuButton(GUI *gui, Vec3 position, int textureWidth, int textureHeight, string textureNormal, string textureHover);
	virtual ~MenuButton();
	void setHighlighted(bool highlighted);
	bool isHighlighted();
	void onMouseClick(Vec3 mousePos);
	void onMouseMove(Vec3 mousePos);
	void pressed();
	void setVisible(bool state);
	void setOnClick(const std::function<void()> &handler);

private:
	bool highlighted;
	std::function<void()> handler;
}; 

class CheckBox : public MenuItem
{
public:
	CheckBox(GUI *gui, Vec3 position, int width = CHECKBOX_TEXTURE_WIDTH, int height = CHECKBOX_TEXTURE_HEIGHT);
	void setOnClick(const std::function<void()> &handler);
	void setChecked(bool checked);
	void onMouseClick(Vec3 mousePos);
	void setVisible(bool state);
	bool isChecked() const;
	

private:
	std::function<void()> handler;
	bool checked;
};

class Slider : public MenuItem
{
public:
	Slider(GUI *gui, Vec3 position, int width = SLIDER_BG_TEXTURE_WIDTH, int height = SLIDER_BG_TEXTURE_HEIGHT);
	void setVisible(bool state);
	void onMouseClick(Vec3 mousePos);
	void onMouseDown(Vec3 mousePos);
	void updateScreenRes(unsigned int x, unsigned int y);
	int getWidth() const;
	int getHeight() const;
	void setValue(float value);  // Value between 0 - 1
	float getValue() const; // Value between 0 - 1
private:
	Rectangle sliderBounds;
	bool mouseDown;
	bool wasClicked;
	float value;
};
#endif 