#include "Components/Checkpoint.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void Goal::update(float dt)
{
	//deactivate
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//sphere test for goal mesh
	Sphere cp = Sphere(getEntityPos(), 5);
	Sphere player = Sphere(getEntityManager()->getEntity(1)->returnPos(), 5);
	if (cp.Test(player))
		getEntityManager()->sendMessageToEntity("Goal", getOwnerId());

}

void Goal::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //event managment
	}
	activated = 1;

	getOwner()->getModules()->sound->PlaySound("doink-doink.wav", 0.75f);
}

void Goal::Deactivate()
{
	getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId()); //event managment
}