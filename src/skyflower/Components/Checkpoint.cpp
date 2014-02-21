#include "Components/Checkpoint.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

Checkpoint::Checkpoint(Vec3 spawnpoint) : Component("Checkpoint")
{
	this->spawnpoint = spawnpoint;
	activated = 0;
}

Checkpoint::~Checkpoint()
{
}

void Checkpoint::addedToEntity()
{
	requestMessage("Ground", &Checkpoint::Activate);
	requestMessage("Wall", &Checkpoint::Activate);
	requestMessage("Checkpoint", &Checkpoint::Activate);

	particleSystem = getOwner()->getModules()->graphics->CreateParticleSystem();
	particleSystem->SetActive(false);
	particleSystem->SetParticleType(ParticleType::PT_PARTICLE);
	particleSystem->SetParticleAgeLimit(0.25f);
	particleSystem->SetEmitFrequency(1.0f / 50.0f);
	particleSystem->SetEmitDir(XMFLOAT3(0.0f, 1.0f, 0.0f));
	particleSystem->SetConstantAccel(XMFLOAT3(0.0f, 100.0f, 0.0f));
	particleSystem->SetScale(XMFLOAT2(4.0f, 4.0f));
}

void Checkpoint::removeFromEntity()
{
	getOwner()->getModules()->graphics->DeleteParticleSystem(particleSystem);
}

void Checkpoint::update(float dt)
{
	//deactivate
	if (activated > 0)
	{
		activated -= dt;
		if (activated <= 0)
			Deactivate();
	}

	//sphere test for flower checkpoint mesh
	Sphere cp = Sphere(getEntityPos(), 5);
	Sphere player = Sphere(getEntityManager()->getEntity(1)->returnPos(), 5);

	if (cp.Test(player))
		getEntityManager()->sendMessageToEntity("Checkpoint", getOwnerId());

	MorphAnimation *animation = getOwner()->getComponent<MorphAnimation*>("MorphAnimation");

	if (animation)
	{
		if (animation->getWeights() == Vec3(1.0f, 0.0f, 0.0f))
		{
			animation->startMorphing(Vec3(0.0f, 1.0f, 0.0f), 1.0f);
		}
		else if (animation->getWeights() == Vec3(0.0f, 1.0f, 0.0f))
		{
			particleSystem->SetActive(false);

			if (getEntityManager()->getEntity(1)->spawnpos != spawnpoint)
			{
				// Some other checkpoint got activated, morph back to the unactivated mesh.
				animation->startMorphing(Vec3(0.0f, 0.0f, 0.0f), 1.0f);
			}
		}
	}
}

void Checkpoint::Activate(Message const& msg)
{
	if (activated <= 0)
	{
		if (getEntityManager()->getEntity(1)->spawnpos != spawnpoint)
		{
			MorphAnimation *animation = getOwner()->getComponent<MorphAnimation*>("MorphAnimation");

			if (animation)
				animation->startMorphing(Vec3(1, 0, 0), 3.0f);

			getEntityManager()->getEntity(1)->spawnpos = spawnpoint; // set player spawn
			getEntityManager()->sendMessageToEntity("Activated", getOwnerId()); //event managment

			// Update and activate particle system.
			Vec3 pos = getEntityPos();
			pos.Y += 10.0f;
			pos.Z += 1.0f;

			particleSystem->SetEmitPos(XMFLOAT3(pos.X, pos.Y, pos.Z));
			particleSystem->SetActive(true);

			Vec3 position = getOwner()->returnPos();
			getOwner()->getModules()->sound->PlaySound("checkpoint_crystal.wav", 1.0f, &position.X);
			//getOwner()->getModules()->sound->PlaySound("checkpoint_yell.wav", 1.0f, &position.X);
		}
	}

	activated = 0.2f;
}

void Checkpoint::Deactivate()
{
	getEntityManager()->sendMessageToEntity("Deactivated", getOwnerId()); //event managment
}