#include "Components/Touch.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void Touch::update(float dt)
{
	//deactivate
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}


	findActivator();

	if (activator)
		Activate();
}

void Touch::Activate()
{
	if (activated <= 0)
		getOwner()->sendMessage("Activated", this); //event managment

	activated = 0.2f;
}

void Touch::Deactivate()
{
	getOwner()->sendMessage("Deactivated", this); //event managment
}

void Touch::findActivator()
{
	activator = nullptr;
	for (int i = 0; i < getEntityManager()->getNrOfEntities(); i++)
	{
		Entity* j = getEntityManager()->getEntityByIndex(i);
		if (j != getOwner())
		{
			if (j->ground == getOwner() || j->wall == getOwner())
				activator = j;
			else if (j->sphere && getOwner()->sphere)
			{
				if (j->sphere->Test(*getOwner()->sphere))
				{
					activator = j;
					break;
				}
			}
		}
	}
}