#include "Components/AI.h"
#include "EntityManager.h"

void AI::addedToEntity()
{
	//cout << "A AI was added to the system." << endl;

	requestMessage("Attack", &AI::Attack);
			requestMessage("StopMoving", &AI::stopMoving);
		requestMessage("StartMoving", &AI::startMoving);

	if (!getOwner()->field)
		getOwner()->field = getEntityManager()->modules->potentialField->CreateField(8, 8, getEntityPos());

	target = nullptr;
}

void AI::setTarget(Entity* e)
{
	this->target = e;
}

void AI::update(float dt)
{
	attacktime -= dt;
	if (attacktime < 0)
		centerradius = 20;


	nextattack -= dt;
	if (nextattack < 0)
		sendMessage("Attack");

	Vec3 pos = getOwner()->returnPos();
	Ray r = Ray(pos + Vec3(0, 15, 0), Vec3(0, -30, 0));

	////find target
	//Entity* p = getEntityManager()->getEntity(1); //player id = 0

		
	

	//set fields to deactivate
	if (getOwner()->ground)
	{
		if (getOwner()->ground->field)
			getOwner()->ground->field->Active = false;
	}

	Field* targetField = nullptr;
	if (target)
	{
		targetField = getEntityManager()->modules->potentialField->CreateField(-1000, 1000, target->returnPos()); // set target
		//Field* targetcenter = getEntityManager()->modules->potentialField->CreateField(1000, centerradius, this->target->returnPos());

		if (target->field)
			target->field->Active = false;

		if (target->ground)
			if (target->ground->field)
				target->ground->field->Active = false;
	}

	//find direction to target
	Vec3 dir = getEntityManager()->modules->potentialField->GetDir(pos);


	//re activate after use
	if (getOwner()->ground)
	{
		if (getOwner()->ground->field)
			getOwner()->ground->field->Active = true;
	}
	if (target)
	{
		getEntityManager()->modules->potentialField->DeleteField(targetField);
		//getEntityManager()->modules->potentialField->DeleteField(targetcenter);

		if (target->field)
			target->field->Active = true;

		if (target->ground)
			if (target->ground->field)
				target->ground->field->Active = true;
	}


	//check if direction is safe
	bool safe = false;
	for (size_t i = 0; i < Collision::GetInstance()->GetCollisionInstances().size(); i++)
	{
		Vec3 p = pos + dir * getOwner()->getComponent<Movement*>("Movement")->GetSpeed()*dt*1.5f;
		if (Collision::GetInstance()->GetCollisionInstances()[i]->Test(Ray(p + Vec3(0, 15, 0), Vec3(0, -30, 0))) > 0.0f)
		{
			safe = true;
			break;
		}
	}
	if (!safe)
	{
		if (unsafe[unsafeIndex] != nullptr)
			getEntityManager()->modules->potentialField->DeleteField(unsafe[unsafeIndex]);

		unsafe[unsafeIndex] = getEntityManager()->modules->potentialField->CreateField(5, 5, pos + dir * getOwner()->getComponent<Movement*>("Movement")->GetSpeed()*dt*1.5f);
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
