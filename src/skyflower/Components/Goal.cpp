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
	Sphere cp = Sphere(getOwner()->returnPos(), 5);
	Sphere player = Sphere(getEntityManager()->getEntity(1)->returnPos(), 5);

	if (cp.Test(player))
		getOwner()->sendMessage("Goal", this);

}

void Goal::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		getOwner()->sendMessage("Activated", this); //event managment
		getOwner()->getModules()->sound->PlaySound("player_fanfare.wav", 0.75f);
	}
	activated = 1;
}

void Goal::Deactivate()
{
	getOwner()->sendMessage("Deactivated", this); //event managment
}