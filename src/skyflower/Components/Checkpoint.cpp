#include "Components/Checkpoint.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

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
	Sphere player = Sphere(getEntityManager()->getEntity(1)->returnPos(), 5);

	if (cp.Test(player))
		getEntityManager()->sendMessageToEntity("Checkpoint", getOwnerId());

	MorphAnimation *animation = getOwner()->getComponent<MorphAnimation*>("MorphAnimation");

	if (animation && animation->getWeights() == Vec3(1.0f, 0.0f, 0.0f))
		animation->startMorphing(Vec3(0.0f, 1.0f, 0.0f), 1.0f);
}

void Checkpoint::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		MorphAnimation *animation = getOwner()->getComponent<MorphAnimation*>("MorphAnimation");

		if (animation && getEntityManager()->getEntity(1)->spawnpos != spawnpoint)
			animation->startMorphing(Vec3(1, 0, 0), 3.0f);

		getEntityManager()->getEntity(1)->spawnpos = spawnpoint; // set player spawn
		getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //event managment
	}

	activated = 0.2f;
}

void Checkpoint::Deactivate()
{
	getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId()); //event managment
}