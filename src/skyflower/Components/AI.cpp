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
		getOwner()->field = getEntityManager()->modules->potentialField->CreateField(8, 8, getEntityPos());

	target = nullptr;
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


	/*getEntityManager()->getEntity(8)->field->Active = false;
	getEntityManager()->getEntity(15)->field->Active = false;
	getEntityManager()->getEntity(16)->field->Active = false;
	getEntityManager()->getEntity(12)->field->Active = false;
	getEntityManager()->getEntity(13)->field->Active = false;*/

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
	Collision *collision = getOwner()->getModules()->collision;
	const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

	for (size_t i = 0; i < collisionInstances.size(); i++)
	{
		Vec3 p = pos + dir * 5;
		if (collisionInstances[i]->Test(Ray(p + Vec3(0, 15, 0), Vec3(0, -30, 0))) > 0.0f)
		{
			safe = true;
			break;
		}
	}
	if (!safe)
	{
		if (unsafe[unsafeIndex] != nullptr)
			getEntityManager()->modules->potentialField->DeleteField(unsafe[unsafeIndex]);

		unsafe[unsafeIndex] = getEntityManager()->modules->potentialField->CreateField(5, 5, pos);
		unsafeIndex++;
		unsafeIndex %= 5;
	}



	Vec3 dif = dir - curDir;
	curDir += dif / 10;

	getOwner()->getComponent<Movement*>("Movement")->setCamera(curDir, Vec3(), Vec3());
	if(dif.Length() < 0.5f && safe)
		getEntityManager()->sendMessageToEntity("StartMoveForward", getOwnerId());
	else
		getEntityManager()->sendMessageToEntity("StopMoveForward", getOwnerId());

	if (target)
	{
		if ((target->returnPos() - pos).Length() < 3)
		{
			getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //for scripting
		}
	}
}
