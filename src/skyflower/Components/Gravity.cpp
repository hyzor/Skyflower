#include "shared/util.h"

#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

GravityComponent::GravityComponent() : Component("Gravity")
{
	this->enabled = true;
}

GravityComponent::~GravityComponent()
{
}

void GravityComponent::addedToEntity()
{
	this->p = getOwner()->getPhysics();
	groundNormal = Vec3(0.0f, 1.0f, 0.0f);
	foundGroundNormal = false;

	createRays();
}

void GravityComponent::removeFromEntity()
{
	this->p = NULL;
}

void GravityComponent::update(float dt)
{
	Vec3 pos = getOwner()->returnPos();
	if (this->enabled && !getOwner()->ground)
	{
		this->p->AddGravityCalc(pos);
		getOwner()->updatePos(pos);
	}


	if (prevPos != getOwner()->getRelativePos())
	{
		getOwner()->wall = nullptr;
		Entity *ground = nullptr;

		for (size_t k = 0; k < groundRays.size(); k++)
		{
			isGroundColl[k] = false;

			//test feet and head
			float t = testMove(groundRays[k], getOwner(), ground, true, (int)k);

			//reset jump
			if (t == -1)
			{
				Vec3 vel = getOwner()->mPhysicsEntity->GetVelocity();

				if (vel.Y < 0)
					vel = Vec3();

				getOwner()->mPhysicsEntity->SetVelocity(vel);
				getOwner()->mPhysicsEntity->GetStates()->isJumping = false;
				getOwner()->mPhysicsEntity->GetStates()->isActiveProjectile = false;
			}
			else if (t == 1)
			{
				getOwner()->mPhysicsEntity->SetVelocity(Vec3());
				ground = nullptr;
			}
		}

		getOwner()->changeRelative(ground);


		for (size_t k = 0; k < wallRays.size(); k++)
			testMove(wallRays[k], getOwner(), getOwner()->wall, false, (int)k);

		prevPos = getOwner()->getRelativePos();

		//activate event for ground
		if (getOwner()->ground)
		{
			getOwner()->sendMessage("StopBeingThrown", this);
			if (!getOwner()->hasComponents("Throwable")) // bollar kan inte trycka knappar
				getOwner()->ground->sendMessage("Ground", this);

			//so that you can't jump while falling from something
			getOwner()->sendMessage("notInAir", this);
		}
		else
			getOwner()->sendMessage("inAir", this);

		//activate event for wall
		if (getOwner()->wall)
			getOwner()->wall->sendMessage("Wall", this);


		//sphereCollision(dt);
		calculateGroundNormal(getOwner(), getOwner()->ground);
	}

	if (getOwner()->ground)
		if (!getOwner()->hasComponents("Throwable")) // bollar kan inte trycka knappar
			getOwner()->ground->sendMessage("Ground", this);
}

void GravityComponent::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

bool GravityComponent::isEnabled()
{
	return this->enabled;
}

const Vec3 *GravityComponent::GetGroundNormal()
{
	if (!foundGroundNormal)
		return NULL;

	return &groundNormal;
}

float GravityComponent::testMove(Ray r, Entity* e, Entity* &out, bool groundRay, int index)
{
	//test ray relative to entity
	Vec3 pos = e->returnPos();

	float preLength = r.GetDir().Length();

	float limit = ((preLength - 5)*0.5f) / preLength;
	cout << limit << endl;
	
	if (prevPos.Y > pos.Y)
	{
		Vec3 p = pos;
		Vec3 dir = r.GetDir();
		dir.Y -= prevPos.Y - pos.Y;
		p.Y = prevPos.Y;
		float newLength = dir.Length();
		r.Set(p + r.GetPos(), dir);

		limit = ((preLength-5)*0.5f) / newLength;
	}
	else
		r.SetPos(pos + r.GetPos());


	//create sphere for ray
	Vec3 raypos = r.GetDir();
	raypos /= 2;
	raypos += r.GetPos();
	Sphere rayS = Sphere(raypos, 10); //max line length 20

	//test collision for other collidible entitis
	float col = 0;
	Entity* outEntity = nullptr;
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
	{
		Entity* EntiJ = getEntityManager()->getEntityByIndex(j);
		if (EntiJ->collInst && EntiJ != e)
		{
			Sphere collSphere = EntiJ->collInst->GetSphere();
			collSphere.Radius *= EntiJ->returnScale().X + 0.3f;
			if (collSphere.Test(rayS))
			{
				float t = EntiJ->collInst->Test(r);
				if (t > 0)
				{
					if (t > 0.5f) //feet
					{
						if (col == 0 || t < col)
						{
							col = t;
							outEntity = EntiJ;
						}
					}
					else //head
					{
						if (col == 0 || t > col)
						{
							col = t;
							outEntity = EntiJ;
							break;
						}
					}
				}
			}
		}
	}

	//collision detected
	float dir = 0;
	if (col > limit) //feet
	{
		if (groundRay && r.GetDir().Y * col > r.GetDir().Y + 5)
		{
			float t = (r.GetDir().Y * col) / (r.GetDir().Y + 5);
			Vec3 rDir = r.GetDir();
			e->updatePos(pos - rDir*(1 - t));
			dir = -1;
			out = outEntity;
		}
		else if(!groundRay)
		{
			Vec3 rDir = r.GetDir();
			e->updatePos(pos - rDir*(1 - col));
			dir = -1;
			out = outEntity;
		}
		if(groundRay)
			isGroundColl[index] = true;
	}
	else if (col > 0) //head
	{
		Vec3 rDir = r.GetDir();
		e->updatePos(pos + rDir*(col));
		dir = 1;
		out = outEntity;
	}

	return dir;
}

void GravityComponent::createRays()
{

	if (getOwner()->collInst)
	{
		Box bounds = getOwner()->collInst->GetBox();
		bounds.Position -= getOwner()->collInst->GetPosition();
		bounds.Size *= getOwner()->returnScale();

		Box small = bounds;
		small.Position += Vec3(2, 2, 2) * getOwner()->returnScale();
		small.Size -= Vec3(2, 2, 2) * 2 * getOwner()->returnScale();

		//ground rays
		int amountX = (int)(small.Size.X / (1.5f*getOwner()->returnScale().X));
		for (int kx = 1; kx <= amountX; kx++)
		{
			int amountZ = (int)(small.Size.Z / (1.5f*getOwner()->returnScale().Z));
			for (int kz = 1; kz <= amountZ; kz++)
			{
				Vec3 p = small.Position;
				p.Y = bounds.Size.Y;
				p.X = -small.Size.X / 2 + (small.Size.X / (amountX + 1))*kx;
				p.Z = -small.Size.Z / 2 + (small.Size.Z / (amountZ + 1))*kz;

				groundRays.push_back(Ray(p, Vec3(0, -bounds.Size.Y-5, 0)));
				isGroundColl.push_back(false);
			}
		}

		//wall rays
		int amountY = (int)(small.Size.Y / (2 * getOwner()->returnScale().Y));
		for (int ky = 1; ky <= amountY; ky++)
		{
			float pY = small.Position.Y + (small.Size.Y / (amountY + 1))*ky;

			int amountX = (int)(small.Size.X / (2 * getOwner()->returnScale().X));
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
		groundRays.push_back(Ray(Vec3(0, 10, 0), Vec3(0, -10-5, 0)));
		isGroundColl.push_back(false);

		//feet
		wallRays.push_back(Ray(Vec3(-3, 2, 0), Vec3(6, 0, 0))); // test left and right at feet
		wallRays.push_back(Ray(Vec3(0, 2, -3), Vec3(0, 0, 6))); // test front and back at feet
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 2, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
		wallRays.push_back(Ray(Vec3(-3 * 0.71f, 2, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test

		//head
		if (getOwner()->hasComponents("Input"))
		{
			wallRays.push_back(Ray(Vec3(-3, 8.5f, 0), Vec3(6, 0, 0))); // test left and right at head
			wallRays.push_back(Ray(Vec3(0, 8.5f, -3), Vec3(0, 0, 6))); // test front and back at head
			wallRays.push_back(Ray(Vec3(-3 * 0.71f, 8.5f, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
			wallRays.push_back(Ray(Vec3(-3 * 0.71f, 8.5f, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test
		}
	}
}

void GravityComponent::calculateGroundNormal(Entity* e, Entity* ground)
{
	foundGroundNormal = false;

	if (!ground)
	{
		groundNormal = Vec3(0.0f, 1.0f, 0.0f);
		return;
	}

	Vec3 pos = e->returnPos();

	//rays for each point in the triangle
	Ray left = Ray(Vec3(-3, 5, 3) + pos, Vec3(0, -10, 0));
	Ray right = Ray(Vec3(3, 5, 3) + pos, Vec3(0, -10, 0));
	Ray bottom = Ray(Vec3(0, 5, -3) + pos, Vec3(0, -10, 0));

	//find collision for each ray
	float tLeft = ground->collInst->Test(left);
	float tRight = ground->collInst->Test(right);
	float tBottom = ground->collInst->Test(bottom);

	if (tLeft == 0 || tRight == 0 || tBottom == 0)
	{
		groundNormal = Vec3(0.0f, 1.0f, 0.0f);
		return;
	}

	//calculate triangle
	Vec3 pointL = left.GetDir();
	pointL *= tLeft;
	pointL += left.GetPos();

	Vec3 pointR = right.GetDir();
	pointR *= tRight;
	pointR += right.GetPos();

	Vec3 pointB = bottom.GetDir();
	pointB *= tBottom;
	pointB += bottom.GetPos();

	groundNormal = Triangle(pointL, pointR, pointB).GetNormal();

	foundGroundNormal = true;
}

void GravityComponent::sphereCollision(float dt)
{
	/*if (getOwner()->sphere)
	{
		for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
		{
			Entity* EntiJ = getEntityManager()->getEntityByIndex(j);
			if (EntiJ->sphere && EntiJ != getOwner())
			{
				if (EntiJ->sphere->Test(*getOwner()->sphere))
				{
					Vec3 dist = EntiJ->returnPos() - getOwner()->returnPos();
					//getOwner()->updatePos(getOwner()->returnPos()-dist*2*dt);
				}
			}
		}
	}*/
}


int GravityComponent::NrOfGroundRays()
{
	return (int)groundRays.size();
}
Ray GravityComponent::GroundRay(int index)
{
	return groundRays[index];
}
bool GravityComponent::GroundRayColliding(int index)
{
	return isGroundColl[index];
}
