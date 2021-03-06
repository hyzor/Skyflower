#include "Menu.h"

// Must be included last!
#include "shared/debug.h"

Menu::Menu()
{
	m_active = true;
	status = MenuStatus::none;
	selectedButton = 0;
	settings._showHelpTexts = true;
	settings._isFullscreen = false;
	settings._mouseInverted = false;
	settings._soundVolume = 1.0f;
	settings._mouseSense = 1.0f;
	this->first = true;
}

Menu::~Menu()
{
	for (size_t i = 0; i < MenuPageCount; i++)
	{
		for (unsigned j = 0; j < m_pages[i].buttons.size(); j++)
		{
			delete m_pages[i].buttons[j];
		}
		for (unsigned j = 0; j < m_pages[i].checkboxes.size(); j++)
		{
			delete m_pages[i].checkboxes[j];
		}
		for (unsigned j = 0; j < m_pages[i].sliders.size(); j++)
		{
			delete m_pages[i].sliders[j];
		}
	}
}

void Menu::init(GUI *g, int screenWidth, int screeenHeight, SoundEngine *sound)
{
	guiPtr = g;
	soundEngine = sound;

	width = screenWidth;
	height = screeenHeight;

	scaleX = 1.0f;
	scaleY = 1.0f;

	m_bg = g->CreateGUIElementAndBindTexture(Vec3(0, 0), "Menygrafik\\logoFyraTre.png");
	g->GetGUIElement(m_bg)->SetVisible(false);

	m_credits = -1;

	// FIXME: Hardcoding ;(
	m_instructionsWidth = 729;
	m_instructions = -1;

	settingsBox = g->CreateGUIElementAndBindTexture(Vec3(400, 100), "Menygrafik\\bg_settings.png");
	g->GetGUIElement(settingsBox)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, 0.9f);
	g->GetGUIElement(settingsBox)->GetDrawInput()->scale = XMFLOAT2(40, 40);
	g->GetGUIElement(settingsBox)->SetVisible(false);

	// Main page
	MenuButton *resume = new MenuButton(g, Vec3(30, 250), 253, 78, "buttons/resume.png", "buttons/resume_highlighted.png");
	resume->setOnClick([this]() {buttonResumeClicked();});
	m_pages[MenuPageMain].buttons.push_back(resume);
	
	MenuButton *settings = new MenuButton(g, Vec3(30, 340), 251, 89, "buttons/settings.png", "buttons/settings_highlighted.png");
	settings->setOnClick([this]() { setActivePage(MenuPageSettings); });
	m_pages[MenuPageMain].buttons.push_back(settings);
	
	MenuButton *instructions = new MenuButton(g, Vec3(30, 430), 376, 80, "buttons/instructions.png", "buttons/instructions_highlighted.png");
	instructions->setOnClick([this]() { setActivePage(MenuPageInstructions); });
	m_pages[MenuPageMain].buttons.push_back(instructions);
	
	MenuButton *credits = new MenuButton(g, Vec3(30, 520), 230, 80, "buttons/credits.png", "buttons/credits_highlighted.png");
	credits->setOnClick([this]() { setActivePage(MenuPageCredits); });
	m_pages[MenuPageMain].buttons.push_back(credits);

	MenuButton *exit = new MenuButton(g, Vec3(30, 610), 161, 81, "buttons/exit.png", "buttons/exit_highlighted.png");
	exit->setOnClick([this]() {buttonExitClicked(); });
	m_pages[MenuPageMain].buttons.push_back(exit);

	// Settings page
	MenuButton *back = new MenuButton(g, Vec3(30, 250), 174, 80, "buttons/back.png", "buttons/back_highlighted.png");
	back->setOnClick([this]()
	{
		setActivePage(this->isFirst()? MenuPageStart : MenuPageMain);
		m_pages[m_activePage].buttons.at(selectedButton)->setHighlighted(true);
	});
	m_pages[MenuPageSettings].buttons.push_back(back);
	CheckBox *helpTexts = new CheckBox(g, Vec3(430, 460));
	helpTexts->setChecked(this->settings._showHelpTexts);
	m_pages[MenuPageSettings].checkboxes.push_back(helpTexts);

	CheckBox *fullScreen = new CheckBox(g, Vec3(430, 430));
	m_pages[MenuPageSettings].checkboxes.push_back(fullScreen);

	CheckBox *mouseInverted = new CheckBox(g, Vec3(430, 400));
	m_pages[MenuPageSettings].checkboxes.push_back(mouseInverted);


	Slider *volume = new Slider(g, Vec3(430, 330), 200);
	volume->setValue(1.0f);
	m_pages[MenuPageSettings].sliders.push_back(volume);

	Slider *mouseSense = new Slider(g, Vec3(430, 260), 200);
	mouseSense->setValue(1.0f / 3.3f);
	m_pages[MenuPageSettings].sliders.push_back(mouseSense);

	// Instructions page
	MenuButton *back_inst = new MenuButton(g, Vec3(30, 250), 174, 80, "buttons/back.png", "buttons/back_highlighted.png");
	back_inst->setOnClick([this]() { setActivePage(this->isFirst()? MenuPageStart : MenuPageMain); });
	m_pages[MenuPageInstructions].buttons.push_back(back_inst);

	// Start menu
	MenuButton *start = new MenuButton(g, Vec3(30, 250), 253, 78, "buttons/start.png", "buttons/start_highlighted.png");
	start->setOnClick([this]()
	{
		setFirst(false);
		buttonResumeClicked();
		setActivePage(MenuPageMain);
	});
	m_pages[MenuPageStart].buttons.push_back(start);

	MenuButton *startSettings = new MenuButton(g, Vec3(30, 340), 251, 89, "buttons/settings.png", "buttons/settings_highlighted.png");
	startSettings->setOnClick([this]() { setActivePage(MenuPageSettings); });
	m_pages[MenuPageStart].buttons.push_back(startSettings);

	MenuButton *startInstructions = new MenuButton(g, Vec3(30, 430), 376, 80, "buttons/instructions.png", "buttons/instructions_highlighted.png");
	startInstructions->setOnClick([this]() { setActivePage(MenuPageInstructions); });
	m_pages[MenuPageStart].buttons.push_back(startInstructions);

	MenuButton *startCredits = new MenuButton(g, Vec3(30, 520), 230, 80, "buttons/credits.png", "buttons/credits_highlighted.png");
	startCredits->setOnClick([this]() { setActivePage(MenuPageCredits); });
	m_pages[MenuPageStart].buttons.push_back(startCredits);

	MenuButton *startExit = new MenuButton(g, Vec3(30, 610), 161, 81, "buttons/exit.png", "buttons/exit_highlighted.png");
	startExit->setOnClick([this]() {buttonExitClicked(); });
	m_pages[MenuPageStart].buttons.push_back(startExit);

	m_activePage = MenuPageStart;
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

void Menu::setActivePage(int page)
{
	m_activePage = page;

	if (m_active)
		setVisible(true);
}

void Menu::draw()
{
	if (m_activePage == MenuPageSettings)
	{
		Vec3 helpTextPos = m_pages[MenuPageSettings].checkboxes.at(0)->getPosition();
		guiPtr->printText("Show Helptexts", (int)(helpTextPos.X + (30 * scaleX)), (int)helpTextPos.Y, Vec3(1.0f, 1.0f, 1.0f), scaleX);

		Vec3 fullScreenPos = m_pages[MenuPageSettings].checkboxes.at(1)->getPosition();
		guiPtr->printText("Fullscreen", (int)(fullScreenPos.X + (30 * scaleX)), (int)fullScreenPos.Y, Vec3(1.0f, 1.0f, 1.0f), scaleX);

		Vec3 mouseInvertPos = m_pages[MenuPageSettings].checkboxes.at(2)->getPosition();
		guiPtr->printText("Invert Camera", (int)(mouseInvertPos.X + 30 * scaleX), (int)mouseInvertPos.Y, Vec3(1.0f, 1.0f, 1.0f), scaleX);

		Vec3 soundVolumePos = m_pages[MenuPageSettings].sliders.at(0)->getPosition();
		int volumeWidth = m_pages[MenuPageSettings].sliders.at(0)->getWidth();
		guiPtr->printText("Sound Volume", (int)(soundVolumePos.X + volumeWidth + 15.0f), (int)soundVolumePos.Y + 10, Vec3(1.0f, 1.0f, 1.0f), scaleX);

		Vec3 mouseSensePos = m_pages[MenuPageSettings].sliders.at(1)->getPosition();
		int senseWidth = m_pages[MenuPageSettings].sliders.at(1)->getWidth();
		guiPtr->printText("Mouse sense", (int)(mouseSensePos.X + senseWidth + 15.0f), (int)mouseSensePos.Y + 10, Vec3(1.0f, 1.0f, 1.0f), scaleX);
	}
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

	selectedButton = (int)((selectedButton + m_pages[m_activePage].buttons.size()) % m_pages[m_activePage].buttons.size());

	if (enter)
	{
		m_pages[m_activePage].buttons.at(selectedButton)->setHighlighted(false);
		m_pages[m_activePage].buttons.at(selectedButton)->pressed();

		selectedButton = 0;
		lastSelected = 0;
	}
	else if (lastSelected != selectedButton)
	{
		m_pages[m_activePage].buttons.at(lastSelected)->setHighlighted(false);
		m_pages[m_activePage].buttons.at(selectedButton)->setHighlighted(true);

		soundEngine->PlaySound("menu/button.wav", 0.5f);
	}
}

void Menu::mousePressed(Vec3 pos)
{
	int page = m_activePage;

	for (auto it = m_pages[page].buttons.begin(); it != m_pages[page].buttons.end(); ++it)
	{
		(*it)->onMouseClick(pos);
	}
	for (auto it = m_pages[page].checkboxes.begin(); it != m_pages[page].checkboxes.end(); ++it)
	{
		(*it)->onMouseClick(pos);
	}
	for (auto it = m_pages[page].sliders.begin(); it != m_pages[page].sliders.end(); ++it)
	{
		(*it)->onMouseClick(pos);
	}

	settings._showHelpTexts = m_pages[MenuPageSettings].checkboxes[0]->isChecked();
	settings._isFullscreen = m_pages[MenuPageSettings].checkboxes[1]->isChecked();
	settings._mouseInverted = m_pages[MenuPageSettings].checkboxes[2]->isChecked();
	settings._soundVolume = m_pages[MenuPageSettings].sliders[0]->getValue();
}

int Menu::getStatus()
{
	return status;
}

int Menu::getActivePage()
{
	return m_activePage;
}

int Menu::isFirst()
{
	return this->first;
}

void Menu::setVisible(bool visible)
{
	if (visible && this->first)
		guiPtr->GetGUIElement(m_bg)->SetVisible(true);
	else
		guiPtr->GetGUIElement(m_bg)->SetVisible(false);

	if (visible && m_activePage == MenuPageInstructions)
	{
		if (m_instructions == -1)
		{
			m_instructions = guiPtr->CreateGUIElementAndBindTexture(Vec3(this->width - m_instructionsWidth - 6.0f, 6.0f), "Menygrafik\\instructions.png");
		}

		guiPtr->GetGUIElement(m_instructions)->SetVisible(true);
	}
	else if (m_instructions != -1)
	{
		guiPtr->GetGUIElement(m_instructions)->SetVisible(false);
	}

	if (visible && m_activePage == MenuPageSettings)
		guiPtr->GetGUIElement(settingsBox)->SetVisible(true);
	else
		guiPtr->GetGUIElement(settingsBox)->SetVisible(false);

	if (visible && m_activePage == MenuPageCredits)
	{
		if (m_credits == -1)
		{
			m_credits = guiPtr->CreateGUIElementAndBindTexture(Vec3(0.0f, 0.0f), "Menygrafik\\credits.png");
			guiPtr->GetGUIElement(m_credits)->GetDrawInput()->scale = XMFLOAT2((float)this->width / 1024, (float)this->height / 768);

			MenuButton *back_credits = new MenuButton(guiPtr, Vec3(30, 250), 174, 80, "buttons/back.png", "buttons/back_highlighted.png");
			back_credits->setOnClick([this]() { setActivePage(this->isFirst()? MenuPageStart : MenuPageMain); });
			m_pages[MenuPageCredits].buttons.push_back(back_credits);
		}

		guiPtr->GetGUIElement(m_credits)->SetVisible(true);
	}
	else if (m_credits != -1)
	{
		guiPtr->GetGUIElement(m_credits)->SetVisible(false);
	}

	for (int i = 0; i < MenuPageCount; i++)
	{
		if (visible && i == m_activePage)
			m_pages[i].setVisible(true);
		else
			m_pages[i].setVisible(false);
	}

	selectedButton = 0;
	
	if (visible)
		m_pages[m_activePage].buttons.at(selectedButton)->setHighlighted(true);

	// Highlight the selected button
	//guiPtr->GetGUIElement(m_pages[m_activePage].buttons.at(selectedButton)->getTextureIDs().at(0))->SetVisible(visible);
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

	for (size_t i = 0; i < MenuPageCount; i++)
	{
		for (unsigned j = 0; j < m_pages[i].buttons.size(); j++)
		{
			m_pages[i].buttons[j]->updateScreenRes(width, height);
		}
		for (unsigned j = 0; j < m_pages[i].checkboxes.size(); j++)
		{
			m_pages[i].checkboxes[j]->updateScreenRes(width, height);
		}
		for (unsigned j = 0; j < m_pages[i].sliders.size(); j++)
		{
			m_pages[i].sliders[j]->updateScreenRes(width, height);
		}
	}

	guiPtr->GetGUIElement(m_bg)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);

	if (m_credits != -1)
		guiPtr->GetGUIElement(m_credits)->GetDrawInput()->scale = XMFLOAT2(scaleX, scaleY);

	XMFLOAT2 oldPos(400, 100);
	guiPtr->GetGUIElement(settingsBox)->GetDrawInput()->pos = XMFLOAT2(oldPos.x *scaleX, oldPos.y*scaleY);
	guiPtr->GetGUIElement(settingsBox)->GetDrawInput()->scale = XMFLOAT2(scaleX*40, scaleY*40);

	if (m_instructions != -1)
		guiPtr->GetGUIElement(m_instructions)->GetDrawInput()->pos = XMFLOAT2(width - m_instructionsWidth - 6.0f, 6.0f);

	this->width = width;
	this->height = height;
}

void Menu::onMouseMove(Vec3 mousePos)
{
	UINT newSelected = selectedButton;

	for (UINT i = 0; i < m_pages[m_activePage].buttons.size(); i++)
	{
		if (m_pages[m_activePage].buttons[i]->getVisible())
		{
			m_pages[m_activePage].buttons[i]->onMouseMove(mousePos);
		}

		m_pages[m_activePage].buttons[i]->isHighlighted();
		if (m_pages[m_activePage].buttons[i]->isHighlighted())
		{
			newSelected = i;			
		}
	}

	if (newSelected != selectedButton)
	{
		selectedButton = newSelected;
		soundEngine->PlaySound("menu/button.wav", 0.2f);
	}
	else
	{
		// no new button has been selected, highlight the last selected button again
		m_pages[m_activePage].buttons[selectedButton]->setHighlighted(true);
	}

}
void Menu::onMouseDown(Vec3 mousePos)
{
	for (auto it = m_pages[m_activePage].sliders.begin(); it != m_pages[m_activePage].sliders.end(); ++it)
	{
		(*it)->onMouseDown(mousePos);
	}

	settings._soundVolume = m_pages[MenuPageSettings].sliders[0]->getValue();
	settings._mouseSense = m_pages[MenuPageSettings].sliders[1]->getValue()*3 + 0.3f; // mouse sense ranges from 0.3 - 3.3
}

void Menu::setFirst(bool status)
{
	this->first = status;
}

//bool Menu::getFirst()
//{
//	return this->first;
//}
