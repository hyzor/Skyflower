#include "Components/Button.h"
#include "EntityManager.h"


void Button::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		moveTo = startPos + Vec3(0, -1*getEntityScale().Y, 0);
		getEntityManager()->sendMessageToEntity("Activated", getOwnerId());
	}

	activated = 0.5f;
}

void Button::Deactivate()
{
	getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId());
	moveTo = startPos;
}