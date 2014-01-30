#ifndef SKYFLOWER_GUI_H
#define SKYFLOWER_GUI_H


//#include "graphics/GraphicsEngine.h"
#include "shared/Vec3.h"
#include <vector>
#include "GUIElement.h"

#define STANDARD_TEXTURE_PATH "..\\..\\content\\Textures\\"

/*
	A container class for GUIElement. Provides a get-function for GUI elements given an id that can be used to alter values -
	for specific GUI element. It also provides a few set-functions to set GUI element specific values without getting a GUIElement first
*/

// TO ADD: ?? Loading all GUI elements via files ??
// TO ADD: ?? Support for drawing text, not just textures ??

class GUI
{
private:
	std::vector<GUIElement*> mGUIElements;
	//std::vector<Text2D> mTexts;
	unsigned int mCurrGUIElementId;

	//TO ADD: Drawing options for the GUI

public:
	//Standard constructors/destructors
	GUI(GraphicsEngine *graphics);
	GUI(const GUI& other);
	~GUI();

	//Draw the GUI
	void Draw();

	//Create a texture, created texture is returned as a pointer
	Texture2D* CreateTexture2D(unsigned int width, unsigned int height);

	//Create a GUI element without binding it to a texture, returns the ID of the GUI element
	int CreateGUIElement(Vec3 pos);

	//Create a GUI element and bind it to a texture or string to a texture, returns the ID of the GUI element. Other than position, sets default values for the GUI element
	int CreateGUIElementAndBindTexture(Vec3 pos, Texture2D* texture);
	int CreateGUIElementAndBindTexture(Vec3 pos, std::string file);

	//Binds a texture and GUI element together by setting appropriate members of the GUIElement to the provided values
	void BindTextureToGUIElement(unsigned int id, Texture2D* texture, std::string file);

	//Not relevant atm
	void CreateTextObject();

	//Destroy the GUI and all its Textures
	void Destroy();
	
	//Upload data to be rendered
	void UploadData(unsigned int id, const void* data);

	//Returns the GUI element with the given id, or a NULL pointer
	GUIElement* GetGUIElement(unsigned int id);

	//Set all elements as not visible
	void HideGUI();

	void printText(wchar_t* text, int x, int y, Vec3 color = Vec3::Zero(), float scale = 1.0f);

private:
	GraphicsEngine *mGraphics;

	//private functions
	GUIElement* _GetGUIElement(unsigned int id);
};

#endif