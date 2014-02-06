#include "Components/AI.h"
#include "EntityManager.h"

void Throwable::update(float deltaTime)
{
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
	{
		Entity* entity = getEntityManager()->getEntity(getEntityManager()->getEntityId(j));
		if (entity->hasComponents("Throw"))
		{
			//if the entities are colliding and if the other entity have the component Throw
			if (entity->sphere->Test(*getOwner()->sphere))
			{
				//entity want to pick up throwable
				if (entity->getComponent<Throw*>("Throw")->getToPickUp() && !isBeingPickedUp)
				{
					entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(true);
					entity->getComponent<Throw*>("Throw")->setToPickUp(false);
					entity->getComponent<Throw*>("Throw")->setHoldingEntityId(getOwnerId());
					isBeingPickedUp = true;
				}
			}
			//check so that we are comparing the correct entities
			if (entity->getComponent<Throw*>("Throw")->getHoldingEntityId() == getOwnerId())
			{
				//entity want to put down throwable
				if (entity->getComponent<Throw*>("Throw")->getToPutDown() && isBeingPickedUp)
				{
					entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(false);
					entity->getComponent<Throw*>("Throw")->setToPutDown(false);
					isBeingPickedUp = false;

					//update throwable position
					Vec3 pos = entity->returnPos();
					Vec3 rot = entity->returnRot();

					Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 10;
					o.Y = 5;

					getOwner()->updateRot(rot);

					updateEntityPos(pos + o);
					getOwner()->getPhysics()->SetVelocity(Vec3());

					entity->getComponent<Throw*>("Throw")->setHoldingEntityId(-1);

					entity->getComponent<Throw*>("Throw")->setHoldingEntityId(getOwnerId());
				}
				else if (entity->getComponent<Throw*>("Throw")->getIsHoldingThrowable() && isBeingPickedUp)
				{
					//entity want to throw throwable
					if (entity->getComponent<Throw*>("Throw")->getToThrow() && isBeingPickedUp)
					{
						entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(false);
						entity->getComponent<Throw*>("Throw")->setToThrow(false);
						isBeingPickedUp = false;
						isBeingThrown = true;
						entity->getComponent<Throw*>("Throw")->setToPutDown(false);
						entity->getComponent<Throw*>("Throw")->setHoldingEntityId(-1);

						//TODO Throwing in Physics!
					}
					else
					{
						//update throwable position
						Vec3 pos = entity->returnPos();
						Vec3 rot = entity->returnRot();

						Vec3 o = Vec3(cosf(-rot.Y - 3.14 / 2), 0, sinf(-rot.Y - 3.14 / 2)).Normalize() * 10;
						o.Y = 5;

						getOwner()->updateRot(rot);

						updateEntityPos(pos + o);
					}
				}
			}
		}
	}
}