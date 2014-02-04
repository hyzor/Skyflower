#include "MenuItems.h"

// Must be included last!
#include "shared/debug.h"

void MenuItem::updateScreenRes(unsigned int x, unsigned int y)
{
	float scaleY, scaleX;

	scaleX = (float)x / origScreenWidth;
	scaleY = (float)y / origScreenHeigh;

	this->bounds._width = (int)(originalWidth * scaleX);
	this->bounds._height = (int)(originalHeight * scaleY);

	this->position.X = origPos.X * scaleX;
	this->position.Y = origPos.Y * scaleY;
	this->bounds._position = position;

	for (size_t i = 0; i < textureIDs.size(); i++)
	{
		guiPtr->GetGUIElement(textureIDs[i])->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);
		guiPtr->GetGUIElement(textureIDs[i])->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);
	}

}

void MenuItem::setScale(float x, float y)
{
	for (size_t i = 0; i < textureIDs.size(); i++)
	{
		guiPtr->GetGUIElement(textureIDs[i])->GetDrawInput()->scale = XMFLOAT2(x, y);
	}

	this->bounds._width = (int)(this->bounds._width * x);
	this->bounds._height = (int)(this->bounds._height * y);
}

void MenuItem::SetPosition(Vec3 pos)
{
	this->position = pos;
	this->bounds._position = pos;
	for (size_t i = 0; i < textureIDs.size(); i++)
	{
		guiPtr->GetGUIElement(textureIDs[i])->GetDrawInput()->pos = XMFLOAT2(pos.X, pos.Y);
	}
}

/* Button */

MenuButton::MenuButton(GUI *g, Vec3 position, int width, int height, string textureNormal, string textureHover)
: MenuItem(g, position, width, height)
{
	textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureNormal));
	textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureHover));
	g->GetGUIElement(textureIDs[0])->SetVisible(false);
	this->highlighted = false;
	this->handler = []() {};
}

MenuButton::~MenuButton()
{
} 

void MenuButton::setVisible(bool state)
{
	if (state)
	{
		if (highlighted)
			guiPtr->GetGUIElement(textureIDs[1])->SetVisible(true);
		else
			guiPtr->GetGUIElement(textureIDs[0])->SetVisible(true);
	}
	else
	{
		guiPtr->GetGUIElement(textureIDs[1])->SetVisible(false);
		guiPtr->GetGUIElement(textureIDs[0])->SetVisible(false);
	}
}

void MenuButton::onMouseClick(Vec3 mousePos)
{
	if (this->bounds._isInside(mousePos))
	{
		if (handler)
		{
			handler();
		}
	}
}

void MenuButton::setHighlighted(bool highlighted)
{
	guiPtr->GetGUIElement(textureIDs[0])->SetVisible(!highlighted);
	guiPtr->GetGUIElement(textureIDs[1])->SetVisible(highlighted);

	this->highlighted = highlighted;
}

bool MenuButton::isHighlighted()
{
	return highlighted;
}

void MenuButton::setOnClick(const std::function<void()> &handler)
{
	this->handler = handler;
}

void MenuButton::pressed()
{
	handler();
}

void MenuButton::onMouseMove(Vec3 mousePos)
{
	if (this->bounds._isInside(mousePos))
		setHighlighted(true);

	else
		setHighlighted(false);
}

/* Checkbox */

CheckBox::CheckBox(GUI *g, Vec3 position, int width, int height, string textureNormal, string textureChecked)
: MenuItem(g, position, width, height)
{
	this->textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureNormal));
	this->textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureChecked));
	g->GetGUIElement(textureIDs[0])->SetVisible(false);
	g->GetGUIElement(textureIDs[1])->SetVisible(false);
	this->checked = false;
	this->handler = []() {};
}

void CheckBox::onMouseClick(Vec3 mousePos)
{
	if (this->bounds._isInside(mousePos))
	{
		checked = !checked;
		guiPtr->GetGUIElement(textureIDs[0])->SetVisible(!checked);
		guiPtr->GetGUIElement(textureIDs[1])->SetVisible(checked);
	}
	if (handler)
	{
		handler();
	}
}

void CheckBox::setOnClick(const std::function<void()> &handler)
{
	this->handler = handler;
}

void CheckBox::setVisible(bool state)
{
	if (state)
	{
		guiPtr->GetGUIElement(textureIDs[0])->SetVisible(!checked);
		guiPtr->GetGUIElement(textureIDs[1])->SetVisible(checked);
	}
	else
	{
		guiPtr->GetGUIElement(textureIDs[0])->SetVisible(false);
		guiPtr->GetGUIElement(textureIDs[1])->SetVisible(false);
	}
}

bool CheckBox::isChecked()
{
	return this->checked;
}


/* Slider */

Slider::Slider(GUI *gui, Vec3 position, int width, int height, string textureBack, string textureSlider)
:MenuItem(gui, position, width, height)
{
	mouseDown = false;
	sliderBounds._position.X = position.X + (bounds._width/2);
	sliderBounds._position.Y = position.Y - 5.0f;

	sliderBounds._width = 20;
	sliderBounds._height = 50;



	textureIDs.push_back(gui->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureBack));
	textureIDs.push_back(gui->CreateGUIElementAndBindTexture(sliderBounds._position, "Menygrafik\\" + textureSlider));
	gui->GetGUIElement(textureIDs[0])->SetVisible(false);
	gui->GetGUIElement(textureIDs[1])->SetVisible(false);
}

void Slider::setVisible(bool state)
{
	guiPtr->GetGUIElement(textureIDs[0])->SetVisible(state);
	guiPtr->GetGUIElement(textureIDs[1])->SetVisible(state);
}

void Slider::onMouseClick(Vec3 mousePos)
{
	if (bounds._isInside(mousePos))
	{
		sliderBounds._position.X = mousePos.X - sliderBounds._width / 2;
		guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos.x = mousePos.X - sliderBounds._width / 2;
		if (sliderBounds._isInside(mousePos))
			wasClicked = true;
		else
			wasClicked = false;
	}
	else
		wasClicked = false;
}

void Slider::onMouseDown(Vec3 mousePos)
{
	if (wasClicked)
	{
		sliderBounds._position.X = mousePos.X;
		if ((sliderBounds._position.X) > (bounds._position.X + bounds._width))
		{
			sliderBounds._position.X = bounds._position.X + bounds._width;
		}
		else if (sliderBounds._position.X < bounds._position.X)
		{
			sliderBounds._position.X = bounds._position.X;
		} 

		guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos.x = sliderBounds._position.X - 10;
	}
}

float Slider::getValue()
{
	float value = (sliderBounds._position.X - bounds._position.X) / width;
	return value;
}

void Slider::setMouseDown(bool state)
{
	mouseDown = state;
	if (!state)
		wasClicked = false;
}