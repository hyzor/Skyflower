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
		downPos = dir*getOwner()->returnScale();
		first = false;
	}

	//controll button deavtivation movement
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//move button animation
	getOwner()->updateRelativePos(getOwner()->getRelativePos() + (moveTo - getOwner()->getRelativePos()) * 10 * dt / (getOwner()->returnScale()*Vec3(1, 0, 1)).Length());
}

void WallButton::Activate(Message const& msg)
{
	if (activated <= 0)
		moveTo = startPos + downPos;

	getOwner()->sendMessage("Activated", this);

	activated = 1.5f;
}

void WallButton::Deactivate()
{
	moveTo = startPos;
}

bool WallButton::isDown()
{
	return (((startPos + downPos) - getOwner()->getRelativePos()).Length() < (getOwner()->returnScale()*Vec3(1,0,1)).Length() *0.69f);
}