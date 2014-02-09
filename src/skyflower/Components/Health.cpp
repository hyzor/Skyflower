#include "shared/util.h"

#include "Components/Health.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void Health::Respawn(Message const& msg)
{
	//create spheres for owner
	Sphere os = Sphere(getOwner()->spawnpos, 5);
	if (getOwner()->collInst)
		os.Radius = getOwner()->collInst->GetSphere().Radius;

	//find if you can spawn
	bool safe = true;
	for (int i = 0; i < getEntityManager()->getNrOfEntities(); i++)
	{
		Entity* e = getEntityManager()->getEntityByIndex(i);
		if (e != getOwner() && e->hasComponents("Movement"))
		{
			//create spheres for entity
			Sphere es = Sphere(e->returnPos(), 5);
			if (e->collInst)
				es = e->collInst->GetSphere();
			
			//check safe
			if (os.Test(es))
			{
				safe = false;
				break;
			}
		}
	}

	if (safe)
	{
		getOwner()->getPhysics()->SetVelocity(Vec3());
		updateEntityPos(getOwner()->spawnpos);
		health = maxHP;
	}
}