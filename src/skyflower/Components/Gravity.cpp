#include "shared/util.h"

#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void GravityComponent::update(float dt)
{
	Vec3 pos = getEntityPos();
	
	if (this->enabled)
	{
		this->p->AddGravityCalc(pos);
	}

	updateEntityPos(pos);

	getOwner()->wall = nullptr;
	Entity *ground = nullptr;

	for (int k = 0; k < (int)groundRays.size(); k++)
	{
		float t = testMove(groundRays[k], getOwner(), ground); //test feet and head
		//reset jump
		if (t == -1)
		{
			Vec3 vel = getOwner()->mPhysicsEntity->GetVelocity();
			if (vel.Y < 0)
			{
				vel.X = 0;
				vel.Y = 0;
				vel.Z = 0;
			}

			getOwner()->mPhysicsEntity->SetVelocity(vel);
			getOwner()->mPhysicsEntity->GetStates()->isJumping = false;
			getOwner()->mPhysicsEntity->GetStates()->isActiveProjectile = false;
			sendMessageToEntity(getOwnerId(), "StopBeingThrown");
		}
		if (t == 1)
		{
			getOwner()->mPhysicsEntity->SetVelocity(Vec3());
			ground = nullptr;
		}
	}
	getOwner()->changeRelative(ground);


	for (size_t k = 0; k < wallRays.size(); k++)
		testMove(wallRays[k], getOwner(), getOwner()->wall);



	//activate event for ground
	if (getOwner()->ground)
	{
		getEntityManager()->sendMessageToEntity("Ground", getOwner()->ground->fId);
		//so that you can't jump while falling from something
		getEntityManager()->sendMessageToEntity("notInAir", getOwnerId());
	}
	else
		getEntityManager()->sendMessageToEntity("inAir", getOwnerId());

	//activate event for wall
	if (getOwner()->wall)
		getEntityManager()->sendMessageToEntity("Wall", getOwner()->wall->fId);



}

float GravityComponent::testMove(Ray r, Entity* e, Entity* &out)
{
	//test ray relative to entity
	Vec3 pos = e->returnPos();
	r.SetPos(pos + r.GetPos());

	//test collision for other collidible entitis
	float col = 0;
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
	{
		Entity* EntiJ = getEntityManager()->getEntityByIndex(j);
		if (EntiJ->collInst && EntiJ != e)
		{
			float t = EntiJ->collInst->Test(r);
			if (t > 0)
			{
				if (t > 0.5f) //feet
				{
					if (col == 0 || t < col)
					{
						col = t;
						out = EntiJ;
					}
				}
				else //head
				{
					if (col == 0 || t > col)
					{
						col = t;
						out = EntiJ;
						break;
					}
				}
			}
		}
	}

	//collision detected
	float dir = 0;
	if (col > 0.5f) //feet
	{
		Vec3 rDir = r.GetDir();
		e->updatePos(pos - rDir*(1 - col));
		dir = -1;
	}
	else if (col > 0) //head
	{
		Vec3 rDir = r.GetDir();
		e->updatePos(pos + rDir*(col));
		dir = 1;
	}

	return dir;
}

void GravityComponent::createRays()
{

	if (getOwner()->collInst)
	{
		Box bounds = getOwner()->collInst->GetBox();
		bounds.Position -= getOwner()->collInst->GetPosition();

		Box small = bounds;
		small.Position += Vec3(2, 2, 2);
		small.Size -= Vec3(2, 2, 2) * 2;

		//ground rays
		int amountX = (int)(small.Size.X / 1.5f);
		for (int kx = 1; kx <= amountX; kx++)
		{
			int amountZ = (int)(small.Size.Z / 1.5f);
			for (int kz = 1; kz <= amountZ; kz++)
			{
				Vec3 p = small.Position;
				p.Y = bounds.Size.Y;
				p.X = -small.Size.X / 2 + (small.Size.X / (amountX + 1))*kx;
				p.Z = -small.Size.Z / 2 + (small.Size.Z / (amountZ + 1))*kz;

				groundRays.push_back(Ray(p, Vec3(0, -bounds.Size.Y, 0)));
			}
		}

		//wall rays
		int amountY = (int)(small.Size.Y / 2);
		for (int ky = 1; ky <= amountY; ky++)
		{
			float pY = small.Position.Y + (small.Size.Y / (amountY + 1))*ky;

			int amountX = (int)(small.Size.X / 2);
			for (int kx = 1; kx <= amountX; kx++)
			{
				float pX = -small.Size.X / 2 + (small.Size.X / (amountX + 1))*kx;
				float pZ = -bounds.Size.Z / 2;

				wallRays.push_back(Ray(Vec3(pX, pY, pZ), Vec3(0, 0, bounds.Size.Z)));
			}

			int amountZ = (int)(small.Size.Z / 2);
			for (int kz = 1; kz <= amountZ; kz++)
			{
				float pZ = -small.Size.Z / 2 + (small.Size.Z / (amountZ + 1))*kz;
				float pX = -bounds.Size.X / 2;

				wallRays.push_back(Ray(Vec3(pX, pY, pZ), Vec3(bounds.Size.X, 0, 0)));
			}
		}
	}
	else
	{
		//body
		groundRays.push_back(Ray(Vec3(0, 10, 0), Vec3(0, -10, 0)));

		//feet
		wallRays.push_back(Ray(Vec3(-3, 2, 0), Vec3(6, 0, 0))); // test left and right at feet
		wallRays.push_back(Ray(Vec3(0, 2, -3), Vec3(0, 0, 6))); // test front and back at feet
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 2, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 2, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test

		//head
		wallRays.push_back(Ray(Vec3(-3, 8.5f, 0), Vec3(6, 0, 0))); // test left and right at head
		wallRays.push_back(Ray(Vec3(0, 8.5f, -3), Vec3(0, 0, 6))); // test front and back at head
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 8.5f, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 8.5f, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test
	}
}

