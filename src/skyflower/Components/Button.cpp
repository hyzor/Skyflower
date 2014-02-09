#include "Components/Button.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void Button::update(float dt)
{
	//run first update
	if (first)
	{
		moveTo = getOwner()->getRelativePos();
		startPos = moveTo;
		downPos = Vec3(0, -getEntityScale().Y, 0);
		first = false;
	}




	//controll button deavtivation movement
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//activate button when button is down
	if (((startPos + downPos) - getOwner()->getRelativePos()).Length() < getEntityScale().Y / 4)
	{
		if (!act)
		{
			act = true;
			getEntityManager()->sendMessageToEntity("Activated", getOwnerId());

			Vec3 position = getOwner()->returnPos();
			getOwner()->getModules()->sound->PlaySound("button/activate.wav", 1.0f, &position.X);
		}
	}
	else if (act)
	{
		act = false;
		getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId());

		Vec3 position = getOwner()->returnPos();
		getOwner()->getModules()->sound->PlaySound("button/activate.wav", 1.0f, &position.X);
	}

	//move button animation
	getOwner()->updateRelativePos(getOwner()->getRelativePos() + (moveTo - getOwner()->getRelativePos()) * 10 * dt / getEntityScale().Y);
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