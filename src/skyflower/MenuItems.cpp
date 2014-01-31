#include "MenuItems.h"

/* Button */

MenuButton::MenuButton(GUI *g, Vec3 position, int width, int height, string textureNormal, string textureHover)
: MenuItem(g, position, width, height)
{
	this->textureNormalID = g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureNormal);
	this->textureHoverID = g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureHover);
	g->GetGUIElement(textureNormalID)->SetVisible(false);

	this->handler = []() {};
}

MenuButton::~MenuButton()
{
} 

vector<int> MenuButton::getTextureIDs()
{
	vector<int> ret;
	ret.push_back(textureHoverID);
	ret.push_back(textureNormalID);
	return ret;
}

void MenuButton::setVisible(bool state)
{
	if (state)
	{
		if (highlighted)
			guiPtr->GetGUIElement(textureHoverID)->SetVisible(true);
		else
			guiPtr->GetGUIElement(textureNormalID)->SetVisible(true);
	}
	else
	{
		guiPtr->GetGUIElement(textureHoverID)->SetVisible(false);
		guiPtr->GetGUIElement(textureNormalID)->SetVisible(false);
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
	guiPtr->GetGUIElement(textureNormalID)->SetVisible(!highlighted);
	guiPtr->GetGUIElement(textureHoverID)->SetVisible(highlighted);
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

void MenuButton::SetPosition(Vec3 pos)
{
	this->position = pos;
	this->bounds._position = pos;
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->pos = XMFLOAT2(pos.X, pos.Y);
	guiPtr->GetGUIElement(textureHoverID)->GetDrawInput()->pos = XMFLOAT2(pos.X, pos.Y);
}

void MenuButton::setScale(float x, float y)
{
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->scale = XMFLOAT2(x, y);
	guiPtr->GetGUIElement(textureHoverID)->GetDrawInput()->scale = XMFLOAT2(x, y);
	this->bounds._width = (int)(this->bounds._width * x);
	this->bounds._height = (int)(this->bounds._height * y);
}

void MenuButton::updateScreenRes(unsigned int x, unsigned int y)
{
	float scaleY, scaleX;

	scaleX = (float)x / origScreenWidth;
	scaleY = (float)y / origScreenHeigh;

	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);
	guiPtr->GetGUIElement(textureHoverID)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);

	this->bounds._width = (int)(originalWidth * scaleX);
	this->bounds._height = (int)(originalHeight * scaleY);

	this->position.X = origPos.X * scaleX;
	this->position.Y = origPos.Y * scaleY;
	this->bounds._position = position;
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);
	guiPtr->GetGUIElement(textureHoverID)->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);
}

/* Checkbox */

CheckBox::CheckBox(GUI *g, Vec3 position, int width, int height, string textureNormal, string textureChecked)
: MenuItem(g, position, width, height)
{
	this->textureNormalID = g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureNormal);
	this->textureCheckedID = g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureChecked);
	g->GetGUIElement(textureNormalID)->SetVisible(false);
	g->GetGUIElement(textureCheckedID)->SetVisible(false);

	this->handler = []() {};
}

vector<int> CheckBox::getTextureIDs()
{
	vector<int> ret;
	ret.push_back(textureNormalID);
	ret.push_back(textureCheckedID);
	return ret;
}

void CheckBox::onMouseClick(Vec3 mousePos)
{
	if (this->bounds._isInside(mousePos))
	{
		checked = !checked;
		guiPtr->GetGUIElement(textureNormalID)->SetVisible(!checked);
		guiPtr->GetGUIElement(textureCheckedID)->SetVisible(checked);
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
		guiPtr->GetGUIElement(textureNormalID)->SetVisible(!checked);
		guiPtr->GetGUIElement(textureCheckedID)->SetVisible(checked);
	}
	else
	{
		guiPtr->GetGUIElement(textureNormalID)->SetVisible(false);
		guiPtr->GetGUIElement(textureCheckedID)->SetVisible(false);
	}
}

bool CheckBox::isChecked()
{
	return this->checked;
}


void CheckBox::SetPosition(Vec3 pos)
{
	this->position = pos;
	this->bounds._position = pos;
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->pos = XMFLOAT2(pos.X, pos.Y);
	guiPtr->GetGUIElement(textureCheckedID)->GetDrawInput()->pos = XMFLOAT2(pos.X, pos.Y);
}

void CheckBox::setScale(float x, float y)
{
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->scale = XMFLOAT2(x, y);
	guiPtr->GetGUIElement(textureCheckedID)->GetDrawInput()->scale = XMFLOAT2(x, y);
	this->bounds._width = (int)(this->bounds._width * x);
	this->bounds._height = (int)(this->bounds._height * y);
}

void CheckBox::updateScreenRes(unsigned int x, unsigned int y)
{
	float scaleY, scaleX;

	scaleX = (float)x / origScreenWidth;
	scaleY = (float)y / origScreenHeigh;

	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);
	guiPtr->GetGUIElement(textureCheckedID)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);

	this->bounds._width = (int)(originalWidth * scaleX);
	this->bounds._height = (int)(originalHeight * scaleY);

	this->position.X = origPos.X * scaleX;
	this->position.Y = origPos.Y * scaleY;
	this->bounds._position = position;
	guiPtr->GetGUIElement(textureNormalID)->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);
	guiPtr->GetGUIElement(textureCheckedID)->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);
}