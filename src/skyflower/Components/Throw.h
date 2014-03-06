#ifndef THROW_H
#define THROW_H

#include "Cistron.h"
#include "Entity.h"
#include "Components/Throwable.h"
#include "graphics/ParticleSystem.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Cistron;

#define THROW_NUM_PARTICLE_SYSTEMS 5

struct ThrowAimParticleSystem
{
	ParticleSystem *particleSystem;
	Vec3 velocity;
	Vec3 position;
};

class Throw : public Component {

public:
	Throw(bool haveAim);
	virtual ~Throw();

	void addedToEntity();
	void removeFromEntity();
	void update(float dt);
	void ThrowAt(Entity* e);
	void ThrowPlayer();
	void PutDown();
	void PickUp(Entity* e);
	void PickUpAll();

	Entity* getHeldEntity();

private:

	bool toPickUp;
	bool isDizzy;

	Entity* heldEntity;

	int nextAimParticleSystemIndex;
	float nextAimParticleSystemTime;
	ThrowAimParticleSystem aimParticleSystems[THROW_NUM_PARTICLE_SYSTEMS];

	void hideAim();
	void pickUp(Message const & msg);
	void pickUpStop(Message const & msg);
	void Throwing(Message const & msg);
	void dropThrowable(Message const & msg);

	void setIsDizzy(Message const &msg);
	void setNotDizzy(Message const & msg);

	Vec3 getAimDirection();
	void updateAim(float deltaTime);
	
};

#endif