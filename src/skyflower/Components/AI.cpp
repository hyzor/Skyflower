#include "Components/AI.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void AI::addedToEntity()
{
	//cout << "A AI was added to the system." << endl;

	//requestMessage("Attack", &AI::Attack);
	requestMessage("StopMoving", &AI::stopMoving);
	requestMessage("StartMoving", &AI::startMoving);

	if (!getOwner()->field)
		getOwner()->field = getEntityManager()->modules->potentialField->CreateField(8, 8, getOwner()->returnPos());

	target = nullptr;
	getOwner()->sphere->Radius = 3.5f; // AI size
}

void AI::setTarget(Entity* e, float radius)
{
	this->target = e;
	this->centerradius = radius-1;
}

void AI::update(float dt)
{

	Vec3 pos = getOwner()->returnPos();
	Ray r = Ray(pos + Vec3(0, 15, 0), Vec3(0, -30, 0));

	//set fields to deactivate
	getOwner()->ActiveteField(false);
	

	Field* targetField = nullptr;
	Field* targetcenter = nullptr;
	if (target)
	{
		target->ActiveteField(false);
		targetField = getEntityManager()->modules->potentialField->CreateField(-1000, 1000, target->returnPos()); // set target
		targetcenter = getEntityManager()->modules->potentialField->CreateField(1000, centerradius, this->target->returnPos());

	}

	//find direction to target
	Vec3 dir = getEntityManager()->modules->potentialField->GetDir(pos);


	//re activate after use
	getOwner()->ActiveteField(true);
	if (target)
	{
		target->ActiveteField(true);

		getEntityManager()->modules->potentialField->DeleteField(targetField);
		getEntityManager()->modules->potentialField->DeleteField(targetcenter);
	}



	//check if direction is safe
	bool safe = false;
	Collision* collision = getOwner()->getModules()->collision;
	const std::vector<CollisionInstance*> &collisionInstances = collision->GetCollisionInstances();

	for (size_t i = 0; i < collisionInstances.size(); i++)
	{
		Vec3 p = pos + dir * 7;
		if (collisionInstances[i]->Test(Ray(p + Vec3(0, 15, 0), Vec3(0, -30, 0))) > 0.0f)
		{
			safe = true;
			break;
		}
	}
	if (!safe)
	{
		if (unsafe[unsafeIndex])
			getEntityManager()->modules->potentialField->DeleteField(unsafe[unsafeIndex]);

		unsafe[unsafeIndex] = getEntityManager()->modules->potentialField->CreateField(8, 8, pos + dir * 5);
		unsafeIndex++;
		unsafeIndex %= 5;
	}



	Vec3 dif = dir - curDir;
	curDir += dif / 10;

	if (dif.Length() < 0.5f && safe)
	{
		getOwner()->getComponent<Movement*>("Movement")->setCamera(dir, Vec3(), Vec3());
		getOwner()->sendMessage("StartMoveForward", this);
	}
	else
	{
		getOwner()->sendMessage("StopMoveForward", this);
	}

	if (target)
	{
		if ((target->returnPos() - pos).Length() < 3)
		{
			getOwner()->sendMessage("Activated", this); //for scripting
		}
	}

	this->collision(dt);
}

void AI::collision(float dt)
{
	Entity* player = getEntityManager()->getEntity(1);
	Vec3 dist = getOwner()->returnPos() - player->returnPos();
	if (dist.Length() < 5)
	{
		dist.Y = 0;
		getOwner()->updatePos(getOwner()->returnPos() + dist.Normalize() * 10 * dt);
		player->updatePos(player->returnPos() - dist * 10 * dt);
	}
}