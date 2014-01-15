#include "Components/Checkpoint.h"
#include "EntityManager.h"

void Checkpoint::update(float dt)
{
	//deactivate
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//sphere test for flower checkpoint mesh
	Sphere cp = Sphere(getEntityPos(), 5);
	Sphere player = Sphere(getEntityManager()->getEntity(0)->returnPos(), 5);
	if (cp.Test(player))
		getEntityManager()->sendMessageToEntity("Checkpoint", getOwnerId());

}

void Checkpoint::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		getEntityManager()->getEntity(0)->spawnpos = spawnpoint; // set player spawn
		getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //event managment
	}

	activated = 0.2f;
}

void Checkpoint::Deactivate()
{
	getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId()); //event managment
}