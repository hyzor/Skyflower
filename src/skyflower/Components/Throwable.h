#ifndef THROWABLE_H
#define THROWABLE_H

#include "Cistron.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

#define THROW_PARTICLE_EMIT_RATE 0.0175f
#define THROW_PARTICLE_ACCELERATION 1.0f
#define THROW_PARTICLE_SCALE 3.5f
#define THROW_PARTICLE_AGE_LIMIT 0.50f
#define THROW_PARTICLE_FADE_TIME 0.30f

class Throwable : public Component {

public:

	Throwable() : Component("Throwable")
	{
		this->isBeingPickedUp = false;
		this->isBeingThrown = false;
		this->throwerId = -1;
		this->targetPos = Vec3();
		this->respawnTime = 5;
	}
	virtual ~Throwable() {};

	void addedToEntity()
	{
		requestMessage("beingThrown", &Throwable::beingThrown);
		requestMessage("stopBeingThrown", &Throwable::stopBeingThrown);
		requestMessage("Dropped", &Throwable::dropped);

		this->p = getOwner()->getPhysics();
		getOwner()->sphere->Radius = 3.5f; // Throwable size

		this->mParticleSystemThrow = getOwner()->getModules()->graphics->CreateParticleSystem();
		this->mParticleSystemThrow->SetActive(false);
		this->mParticleSystemThrow->SetParticleType(ParticleType::PT_PARTICLE);
		this->mParticleSystemThrow->SetEmitFrequency(FLT_MAX);
		this->mParticleSystemThrow->SetParticleAgeLimit(THROW_PARTICLE_AGE_LIMIT);
		this->mParticleSystemThrow->SetScale(XMFLOAT2(THROW_PARTICLE_SCALE, THROW_PARTICLE_SCALE));
		this->mParticleSystemThrow->SetParticleFadeTime(THROW_PARTICLE_FADE_TIME);
		this->mParticleSystemThrow->SetRandomVelocityActive(false);
	}

	void removeFromEntity()
	{
		this->p = NULL;

		getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystemThrow);
		this->mParticleSystemThrow = NULL;
	}

	void setIsBeingThrown(bool state, EntityId throwerId = -1);

	void setIsBeingPickedUp(bool state, EntityId throwerId = -1);
	bool getIsBeingPickedUp();

	bool getIsBeingThrown()
	{
		return isBeingThrown;
	}

	PhysicsEntity*getPhysicsEntity()
	{
		return p;
	}

private:

	bool isBeingPickedUp;
	bool isBeingThrown;
	EntityId throwerId;
	PhysicsEntity* p;
	Vec3 targetPos;
	ParticleSystem *mParticleSystemThrow;
	Vec3 mPrevPos;
	Vec3 mStartThrowPos;
	float mCurrThrowParticleScale;
	float mThrowParticleTimer;

	void update(float deltaTime);
	void setTargetPos(Vec3 pos);
	Vec3 getTargetPos();

	void beingThrown(Message const & msg)
	{
		this->isBeingThrown = true;
	}

	void stopBeingThrown(Message const & msg)
	{
		this->isBeingThrown = false;
	}

	void dropped(Message const & msg)
	{
		this->isBeingPickedUp = false;
	}

	void Respawn(float dt);

	float respawnTime;
};

#endif