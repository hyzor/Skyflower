#include "Components/AI.h"
#include "EntityManager.h"

void AI::addedToEntity()
{
	cout << "A AI was added to the system." << endl;

	requestMessage("Attack", &AI::Attack);

	if (!getOwner()->field)
		getOwner()->field = getEntityManager()->modules->potentialField->CreateField(8, 8, getEntityPos());
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

	//find target
	Entity* p = getEntityManager()->getEntity(0); //player id = 0


	Field* target = getEntityManager()->modules->potentialField->CreateField(-1000, 1000, p->returnPos());
	Field* targetcenter = getEntityManager()->modules->potentialField->CreateField(1000, centerradius, p->returnPos());

	Vec3 dir;
	if (getOwner()->ground)
		dir = getEntityManager()->modules->potentialField->GetDir(pos, getOwner()->ground->collInst, getOwner()->ground->field); //FIX
	else
		dir = getEntityManager()->modules->potentialField->GetDir(pos, nullptr, nullptr);


	Vec3 dif = dir - curDir;
	curDir += dif / 10;

	getOwner()->getComponent<Movement*>("Movement")->setCamera(curDir, Vec3(), Vec3());
	if(dif.Length() < 0.5f)
		getEntityManager()->sendMessageToEntity("StartMoveForward", getOwnerId());
	else
		getEntityManager()->sendMessageToEntity("StopMoveForward", getOwnerId());


	getEntityManager()->modules->potentialField->DeleteField(target);
	getEntityManager()->modules->potentialField->DeleteField(targetcenter);

	if ((p->returnPos() - pos).Length() < 3)
	{
		getEntityManager()->sendMessageToEntity("Push", getOwnerId());
		getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //for scripting
	}
}
