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
	bgID = gui->CreateGUIElementAndBindTexture(Vec3(0, height - 50.0f), "MenyGrafik\\bg_settings.png");
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = width * 0.1f; 	// bg_settings.png is 10x10px so to get a proper size we scale it
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.y = 5.0f;
	gui->GetGUIElement(bgID)->SetVisible(false);

	duckID = gui->CreateGUIElementAndBindTexture(Vec3(0, height - 94.8f), "helpful_duck.png");
	gui->GetGUIElement(duckID)->GetDrawInput()->scale.x = 0.2f;
	gui->GetGUIElement(duckID)->GetDrawInput()->scale.y = 0.2f;
	gui->GetGUIElement(duckID)->SetVisible(false);

}

void HelpText::update(float dt)
{
	if (m_active)
	{
		GUI* gui = getOwner()->getModules()->gui;

		getOwner()->getModules()->graphics->GetWindowResolution(width, height);
		xPos = (width * 0.5f) - (getOwner()->getModules()->graphics->MeassureString(m_text).X*1.5f * 0.5f);

		gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = width * 0.1f;
		gui->GetGUIElement(bgID)->GetDrawInput()->pos.y = height - 50.0f;
		gui->GetGUIElement(duckID)->GetDrawInput()->pos.y = height - 94.8f;

		float distance = (getEntityManager()->getEntity(1)->returnPos() - orig).Length();
		if (distance < m_range)
		{
			if (m_alpha < 1.0f)
				m_alpha += dt;

			gui->GetGUIElement(bgID)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, m_alpha);
			gui->GetGUIElement(bgID)->SetVisible(true);

			gui->GetGUIElement(duckID)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, m_alpha);
			gui->GetGUIElement(duckID)->SetVisible(true);

			gui->printText(m_text, (int)xPos, height - 40, Vec3(0.0f, 1.0f, 0.0f), 1.5f, m_alpha);
		}
		else if (m_alpha > 0.0f)
		{
			m_alpha -= dt;
			gui->GetGUIElement(bgID)->SetVisible(true);
			gui->GetGUIElement(bgID)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, m_alpha);

			gui->GetGUIElement(duckID)->GetDrawInput()->color = XMVectorSet(1.0f, 1.0f, 1.0f, m_alpha);
			gui->GetGUIElement(duckID)->SetVisible(true);

			gui->printText(m_text, (int)xPos, height - 40, Vec3(0.0f, 1.0f, 0.0f), 1.5f, m_alpha);
		}
		else
		{
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