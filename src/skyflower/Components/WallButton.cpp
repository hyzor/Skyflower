#include "Components/WallButton.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void WallButton::update(float dt)
{
	//run first update
	if (first)
	{
		moveTo = getOwner()->getRelativePos();
		startPos = moveTo;
		downPos = dir*getEntityScale();
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
	if (isDown())
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
	getOwner()->updateRelativePos(getOwner()->getRelativePos() + (moveTo - getOwner()->getRelativePos()) * 10 * dt / (getEntityScale()*Vec3(1, 0, 1)).Length());
}

void WallButton::Activate(Message const& msg)
{
	if (activated <= 0)
		moveTo = startPos + downPos;

	activated = 1.5f;
}

void WallButton::Deactivate()
{
	moveTo = startPos;
}

bool WallButton::isDown()
{
	return (((startPos + downPos) - getOwner()->getRelativePos()).Length() < (getEntityScale()*Vec3(1,0,1)).Length() *0.69f);
}