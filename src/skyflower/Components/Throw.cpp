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
	this->heldEntity = nullptr;
	this->aimEntity = nullptr;
	this->toPickUp = false;
	this->isDizzy = false;
}

Throw::~Throw()
{
}

void Throw::addedToEntity()
{
	requestMessage("PickUp", &Throw::pickUp);
	requestMessage("PickUpStart", &Throw::pickUp);
	requestMessage("PickUpStop", &Throw::pickUpStop);
	requestMessage("DropThrowable", &Throw::dropThrowable);
	requestMessage("Throw", &Throw::Throwing);
	requestMessage("isDizzy", &Throw::setIsDizzy);
	requestMessage("notDizzy", &Throw::setNotDizzy);

	this->aimEntity = getEntityManager()->getEntity(5000);
}

void Throw::update(float dt)
{
	if (isDizzy)
	{
		PutDown();
		this->toPickUp = false;

		return;
	}


	// Entity is holding throwable.
	if (this->heldEntity)
	{
		Throwable* throwalble = heldEntity->getComponent<Throwable*>("Throwable");
		if (throwalble && throwalble->getIsBeingPickedUp())
		{
			// Update throwable position.
			Vec3 position = getOwner()->returnPos();
			Vec3 rotation = getOwner()->returnRot();

			Vec3 direction = Vec3(cosf(-rotation.Y - (float)M_PI_2), 0.0f, sinf(-rotation.Y - (float)M_PI_2)).Normalize();
			Vec3 throwablePosition = position + direction * 2.198f;
			throwablePosition.Y += 5.423f;

			this->heldEntity->updateRot(rotation);
			this->heldEntity->updatePos(throwablePosition);
			this->heldEntity->getPhysics()->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));

			updateAim();
		}
	}

	//player pick up
	if(toPickUp)
		PickUpAll();
}

void Throw::ThrowAt(Entity* e)
{
	if (heldEntity)
	{
		Throwable* throwalble = heldEntity->getComponent<Throwable*>("Throwable");
		if (throwalble && throwalble->getIsBeingPickedUp())
		{
			throwalble->setIsBeingPickedUp(false);
			throwalble->setIsBeingThrown(true, getOwnerId());
			throwalble->getPhysicsEntity()->FireProjectileAt(getOwner()->returnPos(), e->returnPos());

			// Make the aim invisible.
			aimEntity->updateVisible(false);

			Vec3 throwablePosition = heldEntity->returnPos();
			getOwner()->getModules()->sound->PlaySound("swish.wav", 1.0f, &throwablePosition.X);

			// Play throw animation
			AnimatedInstance *animatedInstance = getOwner()->getAnimatedInstance();
			if (animatedInstance && getOwnerId() == 1)
				animatedInstance->SetAnimation(7, false);

			this->heldEntity = nullptr;
		}
	}
}


void Throw::ThrowPlayer()
{
	if (heldEntity)
	{
		Throwable* throwalble = heldEntity->getComponent<Throwable*>("Throwable");
		if (throwalble && throwalble->getIsBeingPickedUp())
		{
			throwalble->setIsBeingPickedUp(false);
			throwalble->setIsBeingThrown(true, getOwnerId());


			Vec3 rotation = getOwner()->returnRot();
			float pitch = getOwner()->getModules()->camera->GetPitch();
			Vec3 aimDirection = Vec3(cosf(-rotation.Y - (float)M_PI_2), sinf(-pitch), sinf(-rotation.Y - (float)M_PI_2)).Normalize();
			throwalble->getPhysicsEntity()->FireProjectile(getOwner()->returnPos(), aimDirection * THROW_FORCE);


			// Make the aim invisible.
			aimEntity->updateVisible(false);

			Vec3 throwablePosition = heldEntity->returnPos();
			getOwner()->getModules()->sound->PlaySound("swish.wav", 1.0f, &throwablePosition.X);

			// Play throw animation
			AnimatedInstance *animatedInstance = getOwner()->getAnimatedInstance();
			if (animatedInstance && getOwnerId() == 1)
				animatedInstance->SetAnimation(7, false);

			this->heldEntity = nullptr;
		}
	}
}

void Throw::PutDown()
{
	if (heldEntity)
	{
		Throwable* throwalble = heldEntity->getComponent<Throwable*>("Throwable");
		if (throwalble && throwalble->getIsBeingPickedUp())
		{
			throwalble->setIsBeingPickedUp(false);
			if (this->aimEntity)
				this->aimEntity->updateVisible(false);
			this->heldEntity = nullptr;
		}
	}
}

void Throw::PickUpAll()
{
	for (int i = 0; i < getEntityManager()->getNrOfEntities(); i++)
		PickUp(getEntityManager()->getEntityByIndex(i));
}

void Throw::PickUp(Entity* e)
{
	if (!heldEntity && e != getOwner())
	{
		Sphere* ownerSphere = getOwner()->sphere;
		Throwable* throwableComponent = e->getComponent<Throwable*>("Throwable");
		if (throwableComponent && !throwableComponent->getIsBeingPickedUp() && !throwableComponent->getIsBeingThrown() && e->sphere && e->sphere->Test(*ownerSphere))
		{
			this->heldEntity = e;
			throwableComponent->setIsBeingPickedUp(true, getOwnerId());
		}
	}
}

Entity* Throw::getHeldEntity()
{
	return heldEntity;
}

void Throw::dropThrowable(Message const & msg)
{
	PutDown();
}

void Throw::setAimVisibility(bool state)
{
	this->aimEntity->updateVisible(state);
}

void Throw::pickUp(Message const & msg)
{
	toPickUp = true;
}

void Throw::pickUpStop(Message const & msg)
{
	toPickUp = false;
}

//if the entity should throw something away
void Throw::Throwing(Message const & msg)
{
	if (heldEntity)
		ThrowPlayer();
}

void Throw::setIsDizzy(Message const &msg)
{
	this->isDizzy = true;
}

void Throw::setNotDizzy(Message const & msg)
{
	this->isDizzy = false;
}

void Throw::updateAim()
{
	if (getOwnerId() == 1 && this->aimEntity && aimEntity)
	{
		Collision *collision = getOwner()->getModules()->collision;
		const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

		CollisionInstance *ownerCollision = getOwner()->returnCollision();
		CollisionInstance *heldEntityCollision = this->heldEntity->returnCollision();
		CollisionInstance *collisionInstance;

		Vec3 rotation = getOwner()->returnRot();
		float pitch = getOwner()->getModules()->camera->GetPitch();
		Vec3 aimDirection = Vec3(cosf(-rotation.Y - (float)M_PI_2), sinf(-pitch), sinf(-rotation.Y - (float)M_PI_2)).Normalize();
		Vec3 aimPosition = heldEntity->returnPos();
		Vec3 velocity = aimDirection * THROW_FORCE;

		const float rayLength = 3.0f;

		Vec3 direction = Vec3(cosf(-rotation.Y - (float)M_PI_2), 0.0f, sinf(-rotation.Y - (float)M_PI_2)).Normalize();
		Vec3 rayDirection = Vec3((direction.X > 0.0f ? 1.0f : -1.0f), -1.0f, (direction.Z > 0.0f ? 1.0f : -1.0f)).Normalize();
		Ray ray = Ray(aimPosition, rayDirection * rayLength);

		this->aimEntity->updateVisible(false);

		const float timeStep = 1.0f / 60.0f;
		bool didHit = false;
		int count = 0;

		while (!didHit && aimPosition.Y > -100.0f && count < 50)
		{
			count++;

			// This is basically a hax, ok?
			aimPosition += GRAVITY_DEFAULT * (timeStep * timeStep) * 0.5f;
			velocity += GRAVITY_DEFAULT * timeStep;
			aimPosition += velocity * timeStep;

			ray.SetPos(aimPosition);

			for (size_t i = 0; i < collisionInstances.size(); i++)
			{
				collisionInstance = collisionInstances[i];

				if (collisionInstance == ownerCollision || collisionInstance == heldEntityCollision)
					continue;

				float t = collisionInstance->Test(ray);

				if (t > 0.0f)
				{
					this->aimEntity->updateRot(rotation);
					this->aimEntity->updatePos(aimPosition + rayDirection * rayLength * t);
					this->aimEntity->updateVisible(true);

					didHit = true;
					break;
				}
			}
		}
	}
}