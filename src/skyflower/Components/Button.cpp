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
		downPos = Vec3(0, -getOwner()->returnScale().Y, 0);
		first = false;
		down = true;
	}




	//controll button deavtivation movement
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//activate button when button is down
	if (isDown())
	{
		if (!act)
		{
			act = true;
			getEntityManager()->sendMessageToEntity("Activated", getOwnerId());

			Vec3 position = getOwner()->returnPos();
			getOwner()->getModules()->sound->PlaySound("button/Button_down.wav", 1.0f, &position.X);
		}
	}
	else if (act)
	{
		act = false;
		getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId());

		Vec3 position = getOwner()->returnPos();
		getOwner()->getModules()->sound->PlaySound("button/Button_up.wav", 1.0f, &position.X);
	}

	//move button animation
	getOwner()->updateRelativePos(getOwner()->getRelativePos() + (moveTo - getOwner()->getRelativePos()) * 10 * dt / getOwner()->returnScale().Y);
}

void Button::Activate(Message const& msg)
{
	if (activated <= 0)
		moveTo = startPos + downPos;

	activated = 0.2f;
}

void Button::Deactivate()
{
	if(!toggle)
		moveTo = startPos;
	else
	{
		if (down)
		{
			moveTo = startPos + downPos*0.7f;
			down = false;
		}
		else
		{
			moveTo = startPos;
			down = true;
		}
	}
}

bool Button::isDown()
{
	return (((startPos + downPos) - getOwner()->getRelativePos()).Length() < getOwner()->returnScale().Y *0.69f);
}