#include "Components/AI.h"
#include "EntityManager.h"

void Throwable::update(float deltaTime)
{
	for (int j = 0; j < getEntityManager()->getNrOfEntities(); j++)
	{
		Entity* entity = getEntityManager()->getEntityByIndex(j);

		if (getOwnerId() != entity->fId)
		{
			if (isBeingThrown)
			{
				//check if colliding with player or AI
				if (entity->sphere != NULL && getOwner()->sphere != NULL && entity->sphere->Test(*getOwner()->sphere))
				{
					if (entity->fId != throwerId)
					{
						isBeingThrown = false;
						entity->sendMessage("isDizzy", this);
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

					//o = direction
					Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 2.198f;

					Collision *collision = getOwner()->getModules()->collision;
					const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

					int x, z;
					if (o.X > 0)
					{
						x = 1;
					}
					else
					{
						x = -1;
					}

					if (o.Z)
					{
						z = 1;
					}
					else
					{
						z = -1;
					}

					for (size_t i = 0; i < collisionInstances.size(); i++)
					{
						Vec3 p = pos + o;
						if (collisionInstances[i]->Test(Ray(p + Vec3(0, 0, 0), Vec3(x, -1, z))) > 0.0f)
						{
							getOwner()->getPhysics()->SetVelocity(Vec3());
							throwerId = -1;
							isBeingThrown = false;
							break;
						}
					}
				}
			}

			Throw* throwComp = entity->getComponent<Throw*>("Throw");

			if (throwComp)
			{
				if (!throwComp->getIsDizzy())
				{
					//if the entities are colliding and if the other entity have the component Throw
					if (entity->sphere->Test(*getOwner()->sphere))
					{
						//entity want to pick up throwable
						if (throwComp->getToPickUp() && !isBeingPickedUp)
						{
							throwComp->setIsHoldingThrowable(true);
							throwComp->setToPickUp(false);
							throwComp->setHoldingEntityId(getOwnerId());
							throwerId = throwComp->getOwnerId();
							isBeingPickedUp = true;
							entity->sendMessage("isHoldingThrowable", this);

							//makes the aim visible
							if (throwComp->getHaveAim())
							{
								getEntityManager()->updateEntityVisibility(true, 5000);
							}
						}
					}
					//check so that we are comparing the correct entities
					if (throwComp->getHoldingEntityId() == getOwnerId())
					{
						//entity want to put down throwable
						if (throwComp->getToPutDown() && isBeingPickedUp)
						{
							throwComp->setIsHoldingThrowable(false);
							throwComp->setToPutDown(false);
							isBeingPickedUp = false;

							//update throwable position
							Vec3 pos = entity->returnPos();
							Vec3 rot = entity->returnRot();

							Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 2.198f;
							o.Y = 5.423f;

							getOwner()->updateRot(rot);

							getOwner()->updatePos(pos + o);
							getOwner()->getPhysics()->SetVelocity(Vec3());

							throwComp->setHoldingEntityId(-1);
							//sendMessageToEntity(entity->fId, "canPush");
							if (throwComp->getHaveAim())
							{
								getEntityManager()->updateEntityVisibility(false, 5000);
							}

							throwerId = -1;
						}
						//if the entity is holding throwable
						else if (throwComp->getIsHoldingThrowable() && isBeingPickedUp)
						{
							////entity want to throw throwable
							//if (entity->getComponent<Throw*>("Throw")->getToThrow() && isBeingPickedUp)
							//{
							//	entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(false);
							//	entity->getComponent<Throw*>("Throw")->setToThrow(false);
							//	entity->getComponent<Throw*>("Throw")->setToPutDown(false);
							//	isBeingPickedUp = false;
							//	isBeingThrown = true;
							//	entity->getComponent<Throw*>("Throw")->setToPutDown(false);
							//	entity->getComponent<Throw*>("Throw")->setHoldingEntityId(-1);
							//	throwerId = entity->getComponent<Throw*>("Throw")->getOwnerId();

							//	//makes the aim invisible
							//	if (entity->getComponent<Throw*>("Throw")->getHaveAim())
							//	{
							//		getEntityManager()->updateEntityVisibility(false, 5000);
							//	}

							//	//TODO Throwing in Physics!
							//	p->FireProjectileAt(getOwner()->returnPos(), targetPos);
							//	sendMessageToEntity(entity->fId, "isNotHoldingThrowable");

							//	AnimatedInstance *animatedInstance = entity->getAnimatedInstance();

							//	if (animatedInstance && entity->fId == 1)
							//	{
							//		// Play throw animation
							//		animatedInstance->SetAnimation(7, false);
							//	}
							//}
							//if entity did not throw throwable, update throwable postition in front of entity
							/*else */if (isBeingPickedUp)
							{


								//update throwable position
								Vec3 pos = entity->returnPos();
								Vec3 rot = entity->returnRot();

								Vec3 o = Vec3(cosf(-rot.Y - 3.14f / 2), 0, sinf(-rot.Y - 3.14f / 2)).Normalize() * 2.198f;
								o.Y = 5.423f;

								//prevent ball from flying through stuff.
								Collision *collision = getOwner()->getModules()->collision;
								const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

								int x, z;
								if (o.X > 0)
								{
									x = 4;
								}
								else
								{
									x = -4;
								}

								if (o.Z > 0)
								{
									z = 4;
								}
								else
								{
									z = -4;
								}

								/*
								for (size_t i = 0; i < collisionInstances.size(); i++)
								{
									Vec3 p = pos + o;

									// Test(Vec3 pos, Vec3 direction)
									if (collisionInstances[i]->Test(Ray(p + Vec3(0, 0, 0), Vec3(x, -2, z))) > 0.0f)
									{
										isBeingPickedUp = false;
										entity->getComponent<Throw*>("Throw")->setIsHoldingThrowable(false);
										sendMessageToEntity(entity->fId, "isNotHoldingThrowable");

										getOwner()->getPhysics()->SetVelocity(Vec3());

										if (entity->getComponent<Throw*>("Throw")->getHaveAim())
										{
											getEntityManager()->updateEntityVisibility(false, 5000);
										}
										break;
									}
								}
								*/

								getOwner()->getPhysics()->SetVelocity(Vec3());
								getOwner()->updatePos(pos + o);

								if (throwComp->getHaveAim() && isBeingPickedUp)
								{
									float y = -getOwner()->getModules()->camera->GetPitch();

									Vec3 aim = Vec3(cosf(-rot.Y - 3.14f / 2), sinf(y), sinf(-rot.Y - 3.14f / 2)).Normalize() * 10;

									int maxRange = 20;

									for (size_t i = 0; i < collisionInstances.size(); i++)
									{
										Vec3 p = pos + aim;

										// Test(Vec3 pos, Vec3 direction)
										Ray r = Ray(p + Vec3(0, 10, 0), Vec3(aim.X, aim.Y, aim.Z) * maxRange);
										float t = collisionInstances[i]->Test(r);
										if (t > 0.0f)
										{
											Vec3 dir = r.GetDir();
											Vec3 pos = r.GetPos();
											Vec3 hit = pos + dir * t;

											this->targetPos = hit;
											getEntityManager()->updateEntityVisibility(true, 5000);
											getEntityManager()->updateEntityPos(hit, 5000);

											//entity want to throw throwable
											if (throwComp->getToThrow() && isBeingPickedUp)
											{
												throwComp->setIsHoldingThrowable(false);
												throwComp->setToThrow(false);
												throwComp->setToPutDown(false);
												isBeingPickedUp = false;
												isBeingThrown = true;
												throwComp->setToPutDown(false);
												throwComp->setHoldingEntityId(-1);
												throwerId = throwComp->getOwnerId();

												//makes the aim invisible
												if (throwComp->getHaveAim())
												{
													getEntityManager()->updateEntityVisibility(false, 5000);
												}

												//TODO Throwing in Physics!
												this->p->FireProjectileAt(getOwner()->returnPos(), targetPos);
												//sendMessageToEntity(entity->fId, "canPush");

												AnimatedInstance *animatedInstance = entity->getAnimatedInstance();

												Vec3 position = getOwner()->returnPos();
												getOwner()->getModules()->sound->PlaySound("swish.wav", 1.0f, &position.X);

												if (animatedInstance && entity->fId == 1)
												{
													// Play throw animation
													animatedInstance->SetAnimation(7, false);
												}
											}
											break;
										}
										else
										{
											getEntityManager()->updateEntityVisibility(false, 5000);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Throwable::setTargetPos(Vec3 pos)
{
	this->targetPos = pos;
}

Vec3 Throwable::getTargetPos()
{
	return this->targetPos;
}