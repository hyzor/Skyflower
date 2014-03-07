#include "shared/util.h"

#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

ShadowComp::ShadowComp() : Component("Shadow")
{
	shadowmodel = nullptr;
}

ShadowComp::~ShadowComp()
{
	if (shadowmodel)
		getEntityManager()->modules->graphics->DeleteInstance(shadowmodel);
}

void ShadowComp::addedToEntity()
{
	shadowmodel = getEntityManager()->modules->graphics->CreateInstance("Models\\Shadow\\Shadow");
	shadowmodel->SetVisibility(false);
}

void ShadowComp::removeFromEntity()
{
}

void ShadowComp::update(float dt)
{
	Ray ground = Ray(getOwner()->returnPos() + Vec3(0, 1, 0), Vec3(0, -70, 0));

	float col = 1;
	for (int i = 0; i < getEntityManager()->getNrOfEntities(); i++)
	{
		Entity* e = getEntityManager()->getEntityByIndex(i);
		if (e != getOwner() && e->collInst)
		{
			float t = e->collInst->Test(ground);
			if (t > 0 && t < col)
				col = t;
		}
	}
	if (col != 1)
	{
		Vec3 pos = ground.GetDir();
		pos *= col;
		pos += ground.GetPos();
		shadowmodel->SetPosition(pos);
		shadowmodel->SetVisibility(true);

		float scale = 1 - col;
		shadowmodel->SetScale(Vec3(scale, scale, scale));
	}
	else
		shadowmodel->SetVisibility(false);
}