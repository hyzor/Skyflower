#include "Components/AI.h"
#include "EntityManager.h"

void Throwable::update(float deltaTime)
{
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
	{
		Entity* entity = getEntityManager()->getEntity(getEntityManager()->getEntityId(j));
		
		if (getOwnerId() != entity->fId)
		{
			if (isBeingThrown)
			{
				//check if colliding with player or AI
				if (entity->sphere != NULL && getOwner()->sphere != NULL && entity->sphere->Test(*getOwner()->sphere))
				{
					if (entity->fId != throwerId)
					{
						sendMessageToEntity(entity->fId, "isDizzy");
						p->SetVelocity(p->GetVelocity()*Vec3(0, 1, 0));
						throwerId = -1;
					}
				}
				//dont fly through collidable entities
				else
				{
					//get throwable position
					Vec3 pos = getOwner()->returnPos();
					Vec3 rot = getOwner()->returnRot();


					//collision with ground
					if (getOwner()->ground)
					{
						throwerId = -1;
						isBeingThrown = false;
					}
				}
			}



			if (entity->hasComponents("Throw"))
			{
				if (!entity->getComponent<Throw*>("Throw")->IsDizzy())
				{
					//if the entities are colliding and if the other entity have the component Throw
					if (entity->sphere->Test(*getOwner()->sphere))
					{
						//entity want to pick up throwable
						if (entity->getComponent<Throw*>("Throw")->getToPickUp() && !isBeingPickedUp && !isBeingThrown)
						{
							entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(true);
							entity->getComponent<Throw*>("Throw")->setToPickUp(false);
							entity->getComponent<Throw*>("Throw")->setHoldingEntityId(getOwnerId());
							throwerId = entity->getComponent<Throw*>("Throw")->getOwnerId();
							isBeingPickedUp = true;
							sendMessageToEntity(entity->fId, "isHoldingThrowable");
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

							/*//update throwable position
							Vec3 pos = entity->returnPos();
							Vec3 rot = entity->returnRot();

							Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 2.198f;
							o.Y = 5.423f;

							getOwner()->updateRot(rot);

							updateEntityPos(pos + o);*/
							getOwner()->getPhysics()->SetVelocity(Vec3());

							entity->getComponent<Throw*>("Throw")->setHoldingEntityId(-1);
							sendMessageToEntity(entity->fId, "isNotHoldingThrowable");

							throwerId = -1;
						}
						//if the entity is holding throwable
						else if (entity->getComponent<Throw*>("Throw")->getIsHoldingThrowable() && isBeingPickedUp)
						{
							//entity want to throw throwable
							if (entity->getComponent<Throw*>("Throw")->getToThrow())
							{
								entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(false);
								entity->getComponent<Throw*>("Throw")->setToThrow(false);
								isBeingPickedUp = false;
								isBeingThrown = true;
								entity->getComponent<Throw*>("Throw")->setToPutDown(false);
								entity->getComponent<Throw*>("Throw")->setHoldingEntityId(-1);
								throwerId = entity->getComponent<Throw*>("Throw")->getOwnerId();

								//update throwable position
								Vec3 pos = entity->returnPos();
								Vec3 rot = entity->returnRot();

								float y = -getOwner()->getModules()->camera->GetPitch();

								//Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), sinf(y - 3.14f / 2), sinf(-rot.Y - 3.14f / 2)).Normalize() * 10;
								Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), sinf(-y - 1), sinf(-rot.Y - 3.14f / 2)).Normalize() * 10;

								o.Y *= -3;
								o.X *= o.Y / 2;
								o.Z *= o.Y / 2;

								//TODO Throwing in Physics!
								p->FireProjectile(entity->returnPos(), o);
								sendMessageToEntity(entity->fId, "isNotHoldingThrowable");
							}
							//if entity did not throw throwable, update throwable postition in front of entity
							else
							{
								//update throwable position
								Vec3 pos = entity->returnPos();
								Vec3 rot = entity->returnRot();

								Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 2.198f;
								o.Y = 5.423f;

								getOwner()->getPhysics()->SetVelocity(Vec3());
								updateEntityPos(pos + o);
							}
						}
					}
				}
			}
		}
	}
}