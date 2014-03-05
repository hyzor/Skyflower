#include "MenuItems.h"

// Must be included last!
#include "shared/debug.h"

void MenuItem::updateScreenRes(unsigned int x, unsigned int y)
{
	float scaleY, scaleX;

	scaleX = (float)x / DEFAULT_SCREEN_WIDTH;
	scaleY = (float)y / DEFAULT_SCREEN_HEIGHT;

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

MenuButton::MenuButton(GUI *g, Vec3 position, int textureWidth, int textureHeight, string textureNormal, string textureHover)
: MenuItem(g, position, textureWidth, textureHeight)
{
	textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureNormal));
	textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\" + textureHover));
	g->GetGUIElement(textureIDs[1])->SetVisible(false);
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

bool MenuButton::getVisible()
{
	bool flag = false;
	if (guiPtr->GetGUIElement(textureIDs[0])->GetVisible())
	{
		flag = true;
	}
	if (guiPtr->GetGUIElement(textureIDs[1])->GetVisible())
	{
		flag = true;
	}

	return flag;
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

CheckBox::CheckBox(GUI *g, Vec3 position, int width, int height)
: MenuItem(g, position, width, height)
{
	this->textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\checkbox_unchecked.png"));
	this->textureIDs.push_back(g->CreateGUIElementAndBindTexture(position, "Menygrafik\\checkbox_checked.png"));
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

void CheckBox::setChecked(bool check)
{
	this->checked = check;
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

bool CheckBox::isChecked() const
{
	return this->checked;
}


/* Slider */

Slider::Slider(GUI *gui, Vec3 position, int width, int height)
:MenuItem(gui, position, width, height)
{
	mouseDown = false;
	wasClicked = false;
	sliderBounds._position.X = position.X + (bounds._width*0.5f);
	sliderBounds._position.Y = position.Y - 5.0f;

	sliderBounds._width = SLIDER_TEXTURE_WIDTH;
	sliderBounds._height = SLIDER_TEXTURE_HEIGHT;

	textureIDs.push_back(gui->CreateGUIElementAndBindTexture(position, "Menygrafik\\slider_back.png"));
	textureIDs.push_back(gui->CreateGUIElementAndBindTexture(sliderBounds._position, "Menygrafik\\slider.png"));
	gui->GetGUIElement(textureIDs[0])->SetVisible(false);
	gui->GetGUIElement(textureIDs[1])->SetVisible(false);

	if (width != SLIDER_BG_TEXTURE_WIDTH)
	{
		float scaleX = (float)width / SLIDER_BG_TEXTURE_WIDTH;
		gui->GetGUIElement(textureIDs[0])->GetDrawInput()->scale.x = scaleX;
	}
	if (height != SLIDER_BG_TEXTURE_HEIGHT)
	{
		float scaleY = (float)height / SLIDER_BG_TEXTURE_HEIGHT;
		gui->GetGUIElement(textureIDs[0])->GetDrawInput()->scale.y = scaleY;
	}
}

void Slider::setVisible(bool state)
{
	guiPtr->GetGUIElement(textureIDs[0])->SetVisible(state);
	guiPtr->GetGUIElement(textureIDs[1])->SetVisible(state);
}

void Slider::setValue(float value)
{
	this->value = value;
	sliderBounds._position.X = position.X + (bounds._width*value);
	guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos.x = sliderBounds._position.X - (sliderBounds._width / 2);
}

void Slider::onMouseClick(Vec3 mousePos)
{
	if (bounds._isInside(mousePos) || sliderBounds._isInside(mousePos))
	{
		sliderBounds._position.X = mousePos.X - sliderBounds._width / 2;
		guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos.x = mousePos.X;

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
		guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos.x = sliderBounds._position.X - (sliderBounds._width / 2);
	}
}

float Slider::getValue() const
{
	float value = (sliderBounds._position.X - bounds._position.X) / width;
	if (value < 0.0f)
		value = 0.0f;
	else if (value > 1.0f)
		value = 1.0f;
	return value;
}

int Slider::getWidth() const
{
	return this->bounds._width;
}

int Slider::getHeight() const
{
	return this->bounds._height;
}

void Slider::updateScreenRes(unsigned int x, unsigned int y)
{
	float value = getValue();
	float scaleY, scaleX, rscaleX, rscaleY;

	scaleX = (float)x / DEFAULT_SCREEN_WIDTH;
	scaleY = (float)y / DEFAULT_SCREEN_HEIGHT;

	rscaleX = (float)this->originalWidth / SLIDER_BG_TEXTURE_WIDTH;
	rscaleY = (float)this->originalHeight / SLIDER_BG_TEXTURE_HEIGHT;

	this->bounds._width = (int)(originalWidth * scaleX);
	this->bounds._height = (int)(originalHeight * scaleY);

	this->position.X = origPos.X * scaleX;
	this->position.Y = origPos.Y * scaleY;
	this->bounds._position = position;

	this->sliderBounds._width = (int)(SLIDER_TEXTURE_WIDTH * scaleX);
	this->sliderBounds._height = (int)(SLIDER_TEXTURE_HEIGHT * scaleY);

	Vec3 sliderPos = Vec3(bounds._position.X + (bounds._width * value), origPos.Y - 5.0f);

	this->sliderBounds._position.X = sliderPos.X;
	this->sliderBounds._position.Y = sliderPos.Y * scaleY;

	guiPtr->GetGUIElement(textureIDs[0])->GetDrawInput()->scale = XMFLOAT2(rscaleX*scaleX, rscaleY*scaleY);
	guiPtr->GetGUIElement(textureIDs[0])->GetDrawInput()->pos = XMFLOAT2(position.X, position.Y);

	guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);
	guiPtr->GetGUIElement(textureIDs[1])->GetDrawInput()->pos = XMFLOAT2(sliderBounds._position.X, sliderBounds._position.Y);
}