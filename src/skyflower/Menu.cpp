#include "Menu.h"

Menu::Menu()
{
	m_active = false;
	status = MenuStatus::none;
}

Menu::~Menu()
{

}

void Menu::init(GraphicsEngine *g)
{
	Button start;
	start._tex = "..\\..\\content\\Textures\\Menygrafik\\button_resume.png";
	start._hoverTex = "..\\..\\content\\Textures\\Menygrafik\\button_resume_hover.png";
	start._position = Vec3(112, 100);
	start._active = true;
	m_buttons.push_back(start);

	Button mid;
	mid._tex = "..\\..\\content\\Textures\\Menygrafik\\button_resume.png";
	mid._hoverTex = "..\\..\\content\\Textures\\Menygrafik\\button_resume_hover.png";
	mid._position = Vec3(112, 250);
	mid._active = true;
	m_buttons.push_back(mid);

	Button bot;
	bot._tex = "..\\..\\content\\Textures\\Menygrafik\\button_exit.png";
	bot._hoverTex = "..\\..\\content\\Textures\\Menygrafik\\button_exit_hover.png";
	bot._position = Vec3(112, 400);
	bot._active = true;
	m_buttons.push_back(bot);

	m_bg = "..\\..\\content\\Textures\\Menygrafik\\fyraTreRatio.png";
}

bool Menu::isActive()
{
	return m_active;
}

void Menu::setActive(bool active)
{
	status = MenuStatus::none;
	m_active = active;
}

void Menu::draw(GraphicsEngine *g)
{
	g->Draw2DTextureFile(m_bg, 0, 0);
	for (unsigned int i = 0; i < m_buttons.size(); i++)
	{
		Button current = m_buttons.at(i);
		if (current._active)
		{
			if (i == selectedButton)
				g->Draw2DTextureFile(current._hoverTex, (int)current._position.X+10, (int)current._position.Y-10);
			else
				g->Draw2DTextureFile(current._tex, (int)current._position.X, (int)current._position.Y);
		}
		
	}
}

void Menu::buttonPressed(unsigned short key)
{
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
		if (selectedButton == 0)
			status = MenuStatus::resume;
		else if (selectedButton == 2)
			status = MenuStatus::exit;		
	}
}

int Menu::getStatus()
{
	return status;
}