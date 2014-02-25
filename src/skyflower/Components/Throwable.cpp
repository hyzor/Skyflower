#define _USE_MATH_DEFINES
#include <cmath>

#include "Components/AI.h"
#include "EntityManager.h"

void Throwable::update(float deltaTime)
{
	if (isBeingThrown)
	{
		Entity *owner = getOwner();
		Sphere *ownerSphere = owner->sphere;

		// Check if colliding with wall button.
		if (owner->wall && owner->wall->hasComponents("WallButton"))
        {
			owner->wall->sendMessage("Wall", this);
        }
		// Check if colliding with player or AI.
		else if (ownerSphere != NULL)
		{
			EntityManager *entityManager = getEntityManager();
			int entityCount = entityManager->getNrOfEntities();
			Entity* entity = NULL;

			for (int i = 0; i < entityCount; i++)
			{
				entity = entityManager->getEntityByIndex(i);

				if (entity != owner && entity->fId != throwerId && entity->sphere != NULL && ownerSphere->Test(*entity->sphere))
				{
					isBeingThrown = false;
					entity->sendMessage("isDizzy", this);
					p->SetVelocity(p->GetVelocity() * Vec3(0, 1, 0));
					throwerId = -1;
					break;
				}
			}
		}

		// Get throwable position.
		Vec3 position = owner->returnPos();
		Vec3 rotation = owner->returnRot();

		Vec3 direction = Vec3(cosf(-rotation.Y - (float)M_PI_2), 0.0f, sinf(-rotation.Y - (float)M_PI_2)).Normalize();
		Vec3 throwablePosition = position + direction * 2.198f;

		int x = (direction.X > 0? 1 : -1);
		int z = (direction.Z > 0? 1 : -1);

		Ray ray = Ray(throwablePosition, Vec3(x, -1.0f, z));

		Collision *collision = owner->getModules()->collision;
		const std::vector<CollisionInstance *> &collisionInstances = collision->GetCollisionInstances();

		// Don't fly through collidable entities.
		for (size_t i = 0; i < collisionInstances.size(); i++)
		{
			if (collisionInstances[i]->Test(ray) > 0.0f)
			{
				owner->getPhysics()->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));
				throwerId = -1;
				isBeingThrown = false;
				break;
			}
		}
	}

	if (this->isBeingThrown)
	{
		Vec3 currPos = this->getOwner()->returnPos();

		this->mParticleSystemThrow->SetActive(true);
		this->mParticleSystemThrow->SetEmitPos(XMFLOAT3(currPos.X, currPos.Y + 1.15f, currPos.Z));
		this->mParticleSystemThrow->SetEmitFrequency(THROW_PARTICLE_EMIT_RATE);
		this->mThrowParticleTimer = 0.0f;
	}
	else
	{
		this->mThrowParticleTimer += deltaTime;
		this->mParticleSystemThrow->SetEmitFrequency(FLT_MAX);
		if (this->mThrowParticleTimer > this->mParticleSystemThrow->GetAgeLimit())
		{
			this->mParticleSystemThrow->Reset();
			this->mParticleSystemThrow->SetActive(false);
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

void Throwable::setIsBeingThrown(bool state, EntityId throwerId)
{
	this->isBeingThrown = state;
	this->throwerId = throwerId;
}

void Throwable::setIsBeingPickedUp(bool state, EntityId throwerId)
{
	this->throwerId = throwerId;
	this->isBeingPickedUp = state;
}

bool Throwable::getIsBeingPickedUp()
{
	return isBeingPickedUp;
}
