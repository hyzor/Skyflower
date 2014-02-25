#define _USE_MATH_DEFINES
#include <cmath>

#include "shared/util.h"
#include "Components/Throw.h"
#include "Components/Throwable.h"
#include "EntityManager.h"
//#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

Throw::Throw(bool haveAim) : Component("Throw")
{
	this->heldEntity = NULL;
	this->heldEntityThrowable = NULL;
	this->aimEntity = NULL;
	this->toPickUp = false;
	this->toPutDown = false;
	this->isHoldingThrowable = false;
	this->toThrow = false;
	this->isDizzy = false;
	this->haveAim = haveAim;
}

Throw::~Throw()
{
}

void Throw::addedToEntity()
{
	requestMessage("PickUpOrDown", &Throw::pickUpOrDown);
	requestMessage("PickUp", &Throw::pickUp);
	requestMessage("PickUpStart", &Throw::pickUp);
	requestMessage("PickUpStop", &Throw::pickUpStop);
	requestMessage("Throw", &Throw::Throwing);
	requestMessage("StopThrow", &Throw::stopThrowing);
	requestMessage("DropThrowable", &Throw::dropThrowable);
	requestMessage("isDizzy", &Throw::setIsDizzy);
	requestMessage("notDizzy", &Throw::setNotDizzy);

	this->aimEntity = getEntityManager()->getEntity(5000);
}

void Throw::update(float dt)
{
	if (isDizzy)
	{
		this->heldEntity = NULL;
		this->heldEntityThrowable = NULL;
		this->toPickUp = false;
		this->toPutDown = false;
		this->isHoldingThrowable = false;
		this->toThrow = false;

		return;
	}

	Entity* owner = getOwner();
	EntityManager *entityManager = getEntityManager();

	// Try to pick up throwable.
	if (this->heldEntity == NULL && this->toPickUp)
	{
		Sphere* ownerSphere = owner->sphere;

		int entityCount = entityManager->getNrOfEntities();
		Entity* entity = NULL;
		Throwable* throwableComponent = NULL;

		// Check if we are colliding with throwable.
		for (int i = 0; i < entityCount; i++)
		{
			entity = entityManager->getEntityByIndex(i);

			if (entity == owner)
				continue;

			throwableComponent = entity->getComponent<Throwable*>("Throwable");

			if (throwableComponent && !throwableComponent->getIsBeingPickedUp() && entity->sphere && entity->sphere->Test(*ownerSphere))
			{
				setIsHoldingThrowable(true);
				setToPickUp(false);
				this->heldEntity = entity;
				this->heldEntityThrowable = throwableComponent;
				throwableComponent->setIsBeingPickedUp(true, getOwnerId());
			}
		}
	}
	else if (this->heldEntity != NULL)
	{
		if (this->heldEntityThrowable->getIsBeingPickedUp())
		{
			// Entity want to put down throwable.
			if (getToPutDown())
			{
				setIsHoldingThrowable(false);
				setToPutDown(false);

				this->heldEntityThrowable->setIsBeingPickedUp(false);

				// Update throwable position.
				Vec3 position = owner->returnPos();
				Vec3 rotation = owner->returnRot();

				Vec3 direction = Vec3(cosf(-rotation.Y - (float)M_PI_2), 0.0f, sinf(-rotation.Y - (float)M_PI_2)).Normalize();
				Vec3 throwablePosition = position + direction * 2.198f;
				throwablePosition.Y = 5.423f;

				this->heldEntity->updatePos(throwablePosition);
				this->heldEntity->getPhysics()->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));

				this->heldEntity = NULL;
				this->heldEntityThrowable = NULL;

				//sendMessageToEntity(entity->fId, "canPush");

				if (this->aimEntity && getHaveAim())
				{
					this->aimEntity->updateVisible(false);
				}
			}
			// Entity is holding throwable.
			else if (getIsHoldingThrowable())
			{
				// Update throwable position.
				Vec3 position = owner->returnPos();
				Vec3 rotation = owner->returnRot();

				Vec3 direction = Vec3(cosf(-rotation.Y - (float)M_PI_2), 0.0f, sinf(-rotation.Y - (float)M_PI_2)).Normalize();
				Vec3 throwablePosition = position + direction * 2.198f;
				throwablePosition.Y += 5.423f;

				this->heldEntity->updateRot(rotation);
				this->heldEntity->updatePos(throwablePosition);
				this->heldEntity->getPhysics()->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));

				// Entity want to throw throwable.
				if (getToThrow() && this->aimEntity->returnVisible())
				{
					setIsHoldingThrowable(false);
					setToThrow(false);
					setToPutDown(false);

					this->heldEntityThrowable->setIsBeingPickedUp(false);
					this->heldEntityThrowable->setIsBeingThrown(true, getOwnerId());

					setToPutDown(false);

					this->heldEntityThrowable->getPhysicsEntity()->FireProjectileAt(owner->returnPos(), throwTarget);
					//sendMessageToEntity(entity->fId, "canPush");

					// Make the aim invisible.
					this->aimEntity->updateVisible(false);

					this->heldEntity = NULL;
					this->heldEntityThrowable = NULL;

					AnimatedInstance *animatedInstance = owner->getAnimatedInstance();

					getOwner()->getModules()->sound->PlaySound("swish.wav", 1.0f, &throwablePosition.X);

					if (animatedInstance && getOwnerId() == 1)
					{
						// Play throw animation
						animatedInstance->SetAnimation(7, false);
					}
				}
				// Update aim.
				else if (this->aimEntity && getHaveAim())
				{
					const float maxRange = 200.0f;

					float pitch = owner->getModules()->camera->GetPitch();
					Vec3 aimDirection = Vec3(cosf(-rotation.Y - (float)M_PI_2), sinf(-pitch), sinf(-rotation.Y - (float)M_PI_2)).Normalize();
					Ray ray = Ray(throwablePosition, aimDirection * maxRange);

					CollisionInstance *ownerCollision = owner->returnCollision();
					CollisionInstance *heldEntityCollision = this->heldEntity->returnCollision();

					Collision *collision = owner->getModules()->collision;
					const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();
					CollisionInstance *collisionInstance;

					for (size_t i = 0; i < collisionInstances.size(); i++)
					{
						collisionInstance = collisionInstances[i];

						if (collisionInstance == ownerCollision || collisionInstance == heldEntityCollision)
							continue;

						float t = collisionInstance->Test(ray);

						if (t > 0.0f)
						{
							Vec3 hitPosition = throwablePosition + aimDirection * maxRange * t;

							this->throwTarget = hitPosition;

							this->aimEntity->updateRot(rotation);
							this->aimEntity->updatePos(hitPosition);
							this->aimEntity->updateVisible(true);

							break;
						}
						else
						{
							this->aimEntity->updateVisible(false);
						}
					}
				}
			}
		}
	}
}

Entity* Throw::getHeldEntity()
{
	return this->heldEntity;
}

bool Throw::getToThrow()
{
	return this->toThrow;
}

bool Throw::getIsHoldingThrowable()
{
	return this->isHoldingThrowable;
}

bool Throw::getToPickUp()
{
	return this->toPickUp;
}

bool Throw::getToPutDown()
{
	return this->toPutDown;
}

bool Throw::getIsDizzy()
{
	return isDizzy;
}

void Throw::setToPickUp(bool state)
{
	this->toPickUp = state;
}

void Throw::setToPutDown(bool state)
{
	this->toPutDown = state;
}

bool Throw::getHaveAim()
{
	return this->haveAim;
}

void Throw::setHaveAim(bool state)
{
	this->haveAim = state;
}

void Throw::setIsHoldingThrowable(bool state)
{
	this->isHoldingThrowable = state;
}

void Throw::setToThrow(bool state)
{
	this->toThrow = state;
}

void Throw::dropThrowable(Message const & msg)
{
	if (isHoldingThrowable)
	{
		setToPutDown(false);
		setIsHoldingThrowable(false);
		setToPickUp(false);
		heldEntity->sendMessage("Dropped", this);

		//sendMessageToEntity(getOwnerId(), "canPush");
		if (haveAim && this->aimEntity)
		{
			this->aimEntity->updateVisible(false);
		}

		this->heldEntity = NULL;
		this->heldEntityThrowable = NULL;
	}
}

void Throw::setAimVisibility(bool state)
{
	this->aimEntity->updateVisible(state);
}

//when pick up or put down-button is pressed
void Throw::pickUpOrDown(Message const & msg)
{
	//if the entity is holding something, it should try to put it down
	if (isHoldingThrowable)
	{
		setToPickUp(false);
		setToPutDown(true);
	}
	//if the entity is not holding something, it should try to pick an object
	else
	{
		setToPickUp(true);
		setToPutDown(false);
	}
}

void Throw::pickUp(Message const & msg)
{
	setToPickUp(true);
	setToPutDown(false);
}

void Throw::pickUpStop(Message const & msg)
{
	setToPickUp(false);
}

//if the entity should throw something away
void Throw::Throwing(Message const & msg)
{
	setToThrow(true);
}

//if the entity should stop to try to throw something.
void Throw::stopThrowing(Message const & msg)
{

	setToThrow(false);

}

void Throw::setIsDizzy(Message const &msg)
{
	this->isDizzy = true;
}

void Throw::setNotDizzy(Message const & msg)
{
	this->isDizzy = false;
}