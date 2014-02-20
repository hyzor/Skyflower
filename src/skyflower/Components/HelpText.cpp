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
	UINT x, y;
	x = y = 0;
	//getOwner()->getModules()->graphics->GetWindowResolution(x, y);
	bgID = gui->CreateGUIElementAndBindTexture(Vec3(0, y - 50.0f), "MenyGrafik\\bg_settings.png");
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.x = x * 0.1f; 	// bg_settings.png is 10x10px so to get a proper size we scale it
	gui->GetGUIElement(bgID)->GetDrawInput()->scale.y = 5.0f;
	gui->GetGUIElement(bgID)->SetVisible(false);

	//stringWidth = getOwner()->getModules()->graphics->meassurest
}

void HelpText::update(float dt)
{
	GUI* gui = getOwner()->getModules()->gui;
	if (m_alpha > 0.0f)
	{
		m_alpha -= dt;
		gui->GetGUIElement(bgID)->SetVisible(true);

		//gui->get
		//gui->printText(m_text,)
	}
}

void HelpText::activate(Message const & msg)
{

}
void HelpText::deActivate(Message const & msg)
{

}