#include "Menu.h"

Menu::Menu()
{
	m_active = false;
	status = MenuStatus::none;
	selectedButton = 0;
	settings._isFullscreen = false;
	settings._mouseInverted = false;
	settings._soundVolume = 1.0f;
}

Menu::~Menu()
{
	for (unsigned i = 0; i < m_buttons.size(); i++)
	{
		delete m_buttons.at(i);
	}
}

void Menu::init(GUI *g, int screenWidth, int screeenHeight)
{
	guiPtr = g;

	width = screenWidth;
	height = screeenHeight;

	scaleX = 1.0f;
	scaleY = 1.0f;

	m_bg = g->CreateGUIElementAndBindTexture(Vec3(0, 0), "Menygrafik\\fyraTreRatio.png");
	g->GetGUIElement(m_bg)->SetVisible(false);

	settingsBox = g->CreateGUIElementAndBindTexture(Vec3(400, 100), "Menygrafik\\bg_settings.png");
	g->GetGUIElement(settingsBox)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, 0.9f);
	g->GetGUIElement(settingsBox)->GetDrawInput()->scale = XMFLOAT2(40, 40);
	g->GetGUIElement(settingsBox)->SetVisible(false);

	MenuButton *resume = new MenuButton(g, Vec3(30, 100), 240, 100, "button_resume.png", "button_resume_hover.png");
	resume->setOnClick([this]() {buttonResumeClicked();});
	m_buttons.push_back(resume);

	MenuButton *fillout = new MenuButton(g, Vec3(30, 250), 240, 100, "button_resume.png", "button_resume_hover.png");
	m_buttons.push_back(fillout);

	MenuButton *exit = new MenuButton(g, Vec3(30, 400), 240, 100, "button_exit.png", "button_exit_hover.png");
	exit->setOnClick([this]() {buttonExitClicked(); });
	m_buttons.push_back(exit);

	CheckBox *fullScreen = new CheckBox(g, Vec3(430, 450), 20, 20, "checkbox_unchecked.png", "checkbox_checked.png");
	m_checkboxes.push_back(fullScreen);

	CheckBox *mouseInverted = new CheckBox(g, Vec3(430, 420), 20, 20, "checkbox_unchecked.png", "checkbox_checked.png");
	m_checkboxes.push_back(mouseInverted);
}

bool Menu::isActive()
{
	return m_active;
}

void Menu::setActive(bool active)
{
	status = MenuStatus::none;
	m_active = active;
	setVisible(active);
}

void Menu::draw()
{
	Vec3 fullScreenPos = m_checkboxes.at(0)->getPosition();
	guiPtr->printText(L"Fullscreen", (int)(fullScreenPos.X + (30 * scaleX)), (int)(fullScreenPos.Y), Vec3(1.0f, 1.0f, 1.0f), scaleX);

	Vec3 mouseInvertPos = m_checkboxes.at(1)->getPosition();
	guiPtr->printText(L"Invert Camera", (int)(mouseInvertPos.X + 30 * scaleX), (int)mouseInvertPos.Y, Vec3(1.0f, 1.0f, 1.0f), scaleX);
}

void Menu::keyPressed(unsigned short key)
{
	int lastSelected = selectedButton;
	status = MenuStatus::none;
	bool enter = false;
	switch (key)
	{
	case VK_UP:
		selectedButton--;
		break;
	case VK_DOWN:
		selectedButton++;
		break;
	case VK_RETURN:
		enter = true;
		break;
	}
	if (selectedButton == m_buttons.size())
		selectedButton = 0;
	else if (selectedButton < 0)
		selectedButton = m_buttons.size() - 1;

	if (enter)
	{
		m_buttons.at(selectedButton)->pressed();
	}
	if (lastSelected != selectedButton)
	{
		m_buttons.at(lastSelected)->setHighlighted(false);
		m_buttons.at(selectedButton)->setHighlighted(true);
	}
}

void Menu::mousePressed(Vec3 pos)
{
	for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
	{
		(*it)->onMouseClick(pos);
	}
	for (auto it = m_checkboxes.begin(); it != m_checkboxes.end(); ++it)
	{
		(*it)->onMouseClick(pos);
	}
	settings._isFullscreen = m_checkboxes[0]->isChecked();
	settings._mouseInverted = m_checkboxes[1]->isChecked();
}

int Menu::getStatus()
{
	return status;
}

void Menu::setVisible(bool visible)
{
	guiPtr->GetGUIElement(m_bg)->SetVisible(visible);
	guiPtr->GetGUIElement(settingsBox)->SetVisible(visible);
	for (unsigned i = 0; i < m_buttons.size(); i++)
	{
		m_buttons.at(i)->setVisible(visible);
	}
	for (unsigned i = 0; i < m_checkboxes.size(); i++)
	{
		m_checkboxes.at(i)->setVisible(visible);
	}

	// Highlight the selected button
	guiPtr->GetGUIElement(m_buttons.at(selectedButton)->getTextureIDs().at(0))->SetVisible(visible);
}

void Menu::buttonExitClicked()
{
	status = MenuStatus::exit;
}
void Menu::buttonResumeClicked()
{
	status = MenuStatus::resume;
}

Menu::Settings Menu::getSettings() const
{
	return settings;
}

void Menu::onResize(unsigned int width, unsigned int height)
{
	scaleX = ((float)width / 1024);
	scaleY = ((float)height / 768);

	for (unsigned i = 0; i < m_buttons.size(); i++)
	{
		m_buttons.at(i)->updateScreenRes(width, height);
	}
	for (unsigned i = 0; i < m_checkboxes.size(); i++)
	{
		m_checkboxes.at(i)->updateScreenRes(width, height);
	}

	
	guiPtr->GetGUIElement(m_bg)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);
	XMFLOAT2 oldPos(400, 100);
	guiPtr->GetGUIElement(settingsBox)->GetDrawInput()->pos = XMFLOAT2(oldPos.x *scaleX, oldPos.y*scaleY);
	guiPtr->GetGUIElement(settingsBox)->GetDrawInput()->scale = XMFLOAT2(scaleX*40, scaleY*40);

	this->width = width;
	this->height = height;
}

void Menu::onMouseMove(Vec3 mousePos)
{
	for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
	{
		(*it)->onMouseMove(mousePos);
	}
}
