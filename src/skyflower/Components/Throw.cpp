#define _USE_MATH_DEFINES
#include <cmath>

#include "shared/util.h"
#include "Components/Throw.h"
#include "Components/Throwable.h"
#include "EntityManager.h"
//#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

#define THROW_FORCE 150.0f
#define THROW_PARTICLE_EMIT_FREQUENCY (1.0f / 25.0f)

Throw::Throw(bool haveAim) : Component("Throw")
{
	this->heldEntity = nullptr;
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

	this->nextAimParticleSystemIndex = 0;
	this->nextAimParticleSystemTime = 0.0f;

	if (getOwnerId() == 1)
	{
		for (int i = 0; i < THROW_NUM_PARTICLE_SYSTEMS; i++)
		{
			ParticleSystem *particleSystem = getOwner()->getModules()->graphics->CreateParticleSystem();
			particleSystem->SetActive(true);
			particleSystem->SetParticleType(ParticleType::PT_AIM);
			particleSystem->SetEmitFrequency(FLT_MAX);
			particleSystem->SetParticleAgeLimit(0.75f);
			particleSystem->SetParticleFadeTime(0.75f);
			particleSystem->SetScale(XMFLOAT2(2.0f, 2.0f));
			particleSystem->SetConstantAccel(XMFLOAT3(0.0f, 0.0f, 0.0f));
			particleSystem->SetRandomVelocityActive(false);

			this->aimParticleSystems[i].particleSystem = particleSystem;
		}
	}
}

void Throw::removeFromEntity()
{
	if (getOwnerId() == 1)
	{
		for (int i = 0; i < THROW_NUM_PARTICLE_SYSTEMS; i++)
		{
			getOwner()->getModules()->graphics->DeleteParticleSystem(this->aimParticleSystems[i].particleSystem);
			this->aimParticleSystems[i].particleSystem = NULL;
		}
	}
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

			updateAim(dt);
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

			hideAim();

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

			Vec3 aimDirection = getAimDirection();
			throwalble->getPhysicsEntity()->FireProjectile(getOwner()->returnPos(), aimDirection * THROW_FORCE);

			hideAim();

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
			hideAim();

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

void Throw::hideAim()
{
	if (getOwnerId() != 1)
		return;

	this->nextAimParticleSystemTime = 0.0f;

	for (int i = 0; i < THROW_NUM_PARTICLE_SYSTEMS; i++)
	{
		this->aimParticleSystems[i].particleSystem->SetEmitFrequency(FLT_MAX);
	}
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

Vec3 Throw::getAimDirection()
{
	Vec3 rotation = getOwner()->returnRot();
	float pitch = getOwner()->getModules()->camera->GetPitch();
	pitch -= 0.2f;

	return Vec3(cosf(-rotation.Y - (float)M_PI_2), sinf(-pitch), sinf(-rotation.Y - (float)M_PI_2)).Normalize();
}

void Throw::updateAim(float deltaTime)
{
	if (getOwnerId() != 1)
		return;

	ThrowAimParticleSystem *particleSystem;

	this->nextAimParticleSystemTime -= deltaTime;

	if (this->nextAimParticleSystemTime < 0.0f)
	{
		this->nextAimParticleSystemTime = 0.75f;

		Vec3 aimDirection = getAimDirection();

		particleSystem = &this->aimParticleSystems[this->nextAimParticleSystemIndex];
		particleSystem->particleSystem->SetEmitFrequency(THROW_PARTICLE_EMIT_FREQUENCY);
		particleSystem->position = heldEntity->returnPos();
		particleSystem->velocity = aimDirection * THROW_FORCE;

		this->nextAimParticleSystemIndex = (this->nextAimParticleSystemIndex + 1) % THROW_NUM_PARTICLE_SYSTEMS;
	}

	const float rayLength = 3.0f;

	Collision *collision = getOwner()->getModules()->collision;
	const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

	CollisionInstance *ownerCollision = getOwner()->returnCollision();
	CollisionInstance *heldEntityCollision = this->heldEntity->returnCollision();
	CollisionInstance *collisionInstance;

	Vec3 direction;
	Ray ray;

	for (int i = 0; i < THROW_NUM_PARTICLE_SYSTEMS; i++)
	{
		particleSystem = &this->aimParticleSystems[i];

		if (particleSystem->particleSystem->GetEmitFrequency() == FLT_MAX)
			continue;

		// This is basically a hax, ok?
		particleSystem->position += GRAVITY_DEFAULT * (deltaTime * deltaTime) * 0.5f;
		particleSystem->velocity += GRAVITY_DEFAULT * deltaTime;
		particleSystem->position += particleSystem->velocity * deltaTime;

		if (particleSystem->position.Y < -100.0f)
		{
			particleSystem->particleSystem->SetEmitFrequency(FLT_MAX);
			continue;
		}

		direction = particleSystem->velocity;
		direction.Normalize();

		ray = Ray(particleSystem->position, direction * rayLength);

		for (size_t i = 0; i < collisionInstances.size(); i++)
		{
			collisionInstance = collisionInstances[i];

			if (collisionInstance == ownerCollision || collisionInstance == heldEntityCollision)
				continue;

			float t = collisionInstance->Test(ray);

			if (t > 0.0f)
			{
				particleSystem->particleSystem->SetEmitFrequency(FLT_MAX);
				break;
			}
		}

		particleSystem->particleSystem->SetEmitPos(XMFLOAT3(particleSystem->position.X, particleSystem->position.Y, particleSystem->position.Z));
	}
}