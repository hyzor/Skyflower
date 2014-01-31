#ifndef MENUITEMS_H
#define MENUITEMS_H
#include "graphics\GraphicsEngine.h"
#include <string>
#include <vector>
#include "GUI.h"
#include "GUIElement.h"

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
		this->origScreenWidth = 1024;
		this->origScreenHeigh = 768;
		this->originalWidth = width;
		this->originalHeight = height;
		this->origPos = position;
	}
	virtual ~MenuItem(){};
	virtual vector<int> getTextureIDs() = 0;
	
	virtual Vec3 getPosition()
	{
		return this->position;
	}

protected:
	Vec3 position;
	Rectangle bounds;
	int width, height;
	GUI *guiPtr;
	int origScreenWidth, origScreenHeigh, originalWidth, originalHeight;
	Vec3 origPos;
};

class MenuButton : public MenuItem
{
public:
	
	MenuButton(GUI *gui, Vec3 position, int width, int height, string textureNormal, string textureHover);
	virtual ~MenuButton();
	vector<int> getTextureIDs();
	void setHighlighted(bool highlighted);
	void onMouseClick(Vec3 mousePos);
	void onMouseMove(Vec3 mousePos);
	void pressed();
	void setVisible(bool state);
	void setOnClick(const std::function<void()> &handler);
	void SetPosition(Vec3 pos);
	void setScale(float x, float y);
	void updateScreenRes(unsigned int x, unsigned int y);

private:
	bool highlighted;
	string text;
	int textureHoverID;
	int textureNormalID;


	
	std::function<void()> handler;
}; 

class CheckBox : public MenuItem
{
public:
	CheckBox(GUI *gui, Vec3 position, int width, int height, string textureNormal, string textureChecked);
	void setOnClick(const std::function<void()> &handler);
	vector<int> getTextureIDs();
	void onMouseClick(Vec3 mousePos);
	void setVisible(bool state);
	bool isChecked();
	void SetPosition(Vec3 pos);
	void setScale(float x, float y);
	void updateScreenRes(unsigned int x, unsigned int y);

private:
	std::function<void()> handler;

	bool checked;

	int textureNormalID;
	int textureCheckedID;
};

#endif 