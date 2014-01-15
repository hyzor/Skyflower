#include "Components/Button.h"
#include "EntityManager.h"

void Button::update(float dt)
{
	//controll button deavtivation movement
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//activate button when button is down
	if (((startPos + downPos) - getEntityPos()).Length() < getEntityScale().Y / 4)
	{
		if (!act)
		{
			act = true;
			getEntityManager()->sendMessageToEntity("Activated", getOwnerId());
		}
	}
	else if (act)
	{
		act = false;
		getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId());
	}

	//move button animation
	updateEntityPos(getEntityPos() + (moveTo - getEntityPos()) / 20);
}

void Button::Activate(Message const& msg)
{
	if (activated <= 0)
		moveTo = startPos + downPos;

	activated = 0.2f;
}

void Button::Deactivate()
{
	moveTo = startPos;
}