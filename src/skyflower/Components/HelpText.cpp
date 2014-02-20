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

	requestMessage("MenuActivated", &HelpText::deActivate);
	requestMessage("MenuDeactivated", &HelpText::activate);

	orig = getOwner()->spawnpos;

	GUI* gui = getOwner()->getModules()->gui;

	height = width = 0;
	getOwner()->getModules()->graphics->GetWindowResolution(width, height);
	bgID = gui->CreateGUIElementAndBindTexture(Vec3(0, 0), "MenyGrafik\\bg_settings.png");
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = width * 0.1f; 	// bg_settings.png is 10x10px so to get a proper size we scale it
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.y = 10.0f;
	gui->GetGUIElement(bgID)->SetVisible(false);

	duckID = gui->CreateGUIElementAndBindTexture(Vec3(0, 0), "helpful_duck.png");
	gui->GetGUIElement(duckID)->GetDrawInput()->scale.x = 0.3f;
	gui->GetGUIElement(duckID)->GetDrawInput()->scale.y = 0.3f;
	gui->GetGUIElement(duckID)->SetVisible(false);
	top = 0;
}

void HelpText::update(float dt)
{
	if (m_active)
	{

		GUI* gui = getOwner()->getModules()->gui;

		getOwner()->getModules()->graphics->GetWindowResolution(width, height);
		xPos = (width * 0.5f) - (getOwner()->getModules()->graphics->MeassureString(m_text).X*1.5f * 0.5f);

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

			if (top < 153.6f)
				top += dt*300;

 			gui->GetGUIElement(bgID)->SetVisible(true);
			gui->GetGUIElement(duckID)->SetVisible(true);
			gui->GetGUIElement(bgID)->GetDrawInput()->pos.y = height - top + 85.0f;
			gui->GetGUIElement(duckID)->GetDrawInput()->pos.y = height - top;
			gui->printText(m_text, (int)(xPos), (int)(height - top +108.6f), Vec3(0.0f, 1.0f, 0.0f), 1.5f);
		}
		else if (top > 0.0f)
		{
			top -= dt*300;
			gui->GetGUIElement(bgID)->SetVisible(true);
			gui->GetGUIElement(duckID)->SetVisible(true);

			gui->GetGUIElement(bgID)->GetDrawInput()->pos.y = height - top + 85.0f;
			gui->GetGUIElement(duckID)->GetDrawInput()->pos.y = height - top;
			gui->printText(m_text, (int)(xPos), (int)(height - top + 108.6f), Vec3(0.0f, 1.0f, 0.0f), 1.5f);

		}
		else
		{
			first = true;
			gui->GetGUIElement(bgID)->SetVisible(false);
			gui->GetGUIElement(duckID)->SetVisible(false);
		}
	}
	
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