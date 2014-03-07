#include "shared/util.h"
#include "GUI.h"
#include "Components/HelpText.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void HelpText::addedToEntity()
{
	Entity *owner = getOwner();
	assert(owner);

	requestMessage("MenuActivated", &HelpText::menu_activate);
	requestMessage("MenuDeactivated", &HelpText::menu_deActivate);
	requestMessage("enter pressed", &HelpText::hide);
	requestMessage("Hide helptexts", &HelpText::deActivate);
	requestMessage("Show helptexts", &HelpText::activate);

	orig = getOwner()->spawnpos;
	this->width = 0;
	this->height = 0;
	getOwner()->getModules()->graphics->GetWindowResolution(width, height);

	GUI* gui = getOwner()->getModules()->gui;
	GraphicsEngine* graphics = getOwner()->getModules()->graphics;
	vector<string> split = SplitString(m_text, ' ');
	string tmpStr = "";
	for (unsigned int i = 0; i < split.size(); i++)
	{
		float stringWidth = graphics->MeassureString(tmpStr + split[i]).X;
		if (stringWidth > width*0.5f)
		{
			tmpStr += "\n";
		}
		else
		{
			tmpStr += " ";
		}
		tmpStr += split[i];
	}
	m_text = tmpStr;

	getOwner()->getModules()->graphics->GetWindowResolution(width, height);
	bgID = gui->CreateGUIElementAndBindTexture(Vec3(), "MenyGrafik\\bg_settings.png");
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = width * 0.1f; 	// bg_settings.png is 10x10px so to get a proper size we scale it
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.y = 15.0f;
	gui->GetGUIElement(bgID)->SetVisible(false);

	duckID = gui->CreateGUIElementAndBindTexture(Vec3(), "helpful_duck.png");
	gui->GetGUIElement(duckID)->SetVisible(false);
	top = 0;
}

void HelpText::removeFromEntity()
{
	getOwner()->getModules()->gui->GetGUIElement(bgID)->SetVisible(false);
	getOwner()->getModules()->gui->GetGUIElement(duckID)->SetVisible(false);
	Entity *owner = getOwner();
	assert(owner);
}

void HelpText::update(float dt)
{
	if (m_active && !m_menuActive)
	{
		GUI* gui = getOwner()->getModules()->gui;

		UINT oldW = width;
		UINT oldH = height;
		getOwner()->getModules()->graphics->GetWindowResolution(width, height);

		if (oldW != width || oldH != height)
		{
			GraphicsEngine* graphics = getOwner()->getModules()->graphics;
			vector<string> split = SplitString(m_text, ' ');
			string tmpStr = "";
			for (unsigned int i = 0; i < split.size(); i++)
			{
				float stringWidth = graphics->MeassureString(tmpStr + split[i]).X;
				if (stringWidth > width*0.5f)
				{
					tmpStr += "\n";
				}
				else
				{
					tmpStr += " ";
				}
				tmpStr += split[i];
			}
			m_text = tmpStr;
		}

		xPos = (width * 0.5f) - (getOwner()->getModules()->graphics->MeassureString(m_text).X*1.5f * 0.5f);
		int stringHeight = (int)(getOwner()->getModules()->graphics->MeassureString(m_text).X*1.5f);

		gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = width * 0.1f;

		float distance = (getEntityManager()->getEntity(1)->returnPos() - orig).Length();
		if (distance < m_range)
		{
			if (first)
			{
				first = false;
				// Play sound
				getOwner()->getModules()->sound->PlaySound("comedy_duck.wav", 1.0f);
			}

			if (top < 120.0f)
				top += dt*300;
			else
			{	
				m_timer += dt;
				if (m_timer > 0.02f && m_textToPrint.size() < m_text.size())
				{
					m_timer = 0;
					m_textToPrint += m_text[m_textToPrint.size()];
				}
			}
 			gui->GetGUIElement(bgID)->SetVisible(true);
			gui->GetGUIElement(duckID)->SetVisible(true);
			gui->GetGUIElement(bgID)->GetDrawInput()->pos.y = height - top;
			gui->GetGUIElement(duckID)->GetDrawInput()->pos.y = height - top - 1.0f;
			gui->printText(m_textToPrint, (int)(xPos), (int)(height - top + 20.0f), Vec3(0.0f, 1.0f, 0.0f), 1.5f);

			string close = "(Press enter to close)";
			float cHeight = getOwner()->getModules()->graphics->MeassureString(close).Y * 0.8f;
			float closePos = width * 0.5f - getOwner()->getModules()->graphics->MeassureString(close).X * 0.8f * 0.5f;
			gui->printText(close, (int)closePos, (int)(height - top + (120-cHeight)), Vec3(1.0f, 1.0f, 1.0f), 0.8f);
		}
		else if (top > 0.0f)
		{
			top -= dt*300;
			gui->GetGUIElement(bgID)->SetVisible(true);
			gui->GetGUIElement(duckID)->SetVisible(true);

			gui->GetGUIElement(bgID)->GetDrawInput()->pos.y = height - top;
			gui->GetGUIElement(duckID)->GetDrawInput()->pos.y = height - top - 1.0f;
			gui->printText(m_textToPrint, (int)(xPos), (int)(height - top + 20.0f), Vec3(0.0f, 1.0f, 0.0f), 1.5f);

			string close = "(Press enter to close)";
			float cHeight = getOwner()->getModules()->graphics->MeassureString(close).Y * 0.8f; 
			float closePos = width * 0.5f - getOwner()->getModules()->graphics->MeassureString(close).X * 0.8f * 0.5f;
			gui->printText(close, (int)closePos, (int)(height - top + (120 - cHeight)), Vec3(1.0f, 1.0f, 1.0f), 0.8f);

		}
		else
		{
			top = 0.0f;
			m_textToPrint = "";
			first = true;
			gui->GetGUIElement(bgID)->SetVisible(false);
			gui->GetGUIElement(duckID)->SetVisible(false);
		}
	}
}

vector<string> HelpText::SplitString(string str, char token)
{
	vector<string> ret;
	ret.emplace_back(string());

	int j = 0;
	for (unsigned i = 0; i < str.length(); i++)
	{
		if (str[i] != token)
		{
			ret.at(j) += str[i];
		}
		else
		{
			j++;
			ret.emplace_back(string());
		}
	}
	return ret;
}

// Message handling

void HelpText::menu_activate(Message const & msg)
{
	getOwner()->getModules()->gui->GetGUIElement(bgID)->SetVisible(false);
	getOwner()->getModules()->gui->GetGUIElement(duckID)->SetVisible(false);
	m_menuActive = true;
}
void HelpText::menu_deActivate(Message const & msg)
{
	m_menuActive = false;
}

void HelpText::activate(Message const & msg)
{
	m_active = true;
}
void HelpText::deActivate(Message const & msg)
{
	getOwner()->getModules()->gui->GetGUIElement(bgID)->SetVisible(false);
	getOwner()->getModules()->gui->GetGUIElement(duckID)->SetVisible(false);
	m_active = false;
}
void HelpText::hide(Message const & msg)
{
	float distance = (getEntityManager()->getEntity(1)->returnPos() - orig).Length();
	if (distance < m_range)
	{
		top = 0.0f;
		m_textToPrint = "";
		first = true;
		getOwner()->getModules()->gui->GetGUIElement(bgID)->SetVisible(false);
		getOwner()->getModules()->gui->GetGUIElement(duckID)->SetVisible(false);
		m_active = false;
	}
}