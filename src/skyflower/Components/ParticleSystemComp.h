#ifndef SKYFLOWER_COMPONENTS_PARTICLESYSTEMCOMP_H
#define SKYFLOWER_COMPONENTS_PARTICLESYSTEMCOMP_H

#include "shared/Vec3.h"
#include "graphics/instance.h"
#include "Cistron.h"
#include "Entity.h"
#include "Component.h"

#include <string>

using namespace std;
using namespace Cistron;

class ParticleSystemComp : public Component
{
public:

	ParticleSystemComp(Vec3 initialPosition) : Component("ParticleSystemComp")
	{
		this->mPosition = initialPosition;
	};

	virtual ~ParticleSystemComp(){}

	void addedToEntity();
	void removeFromEntity();
	void update(float deltaTime);

private:

	//Pointer to the actual object that will be rendered by the graphics engine
	ParticleSystem* mParticleSystem;

	//Variables containing data that define the behaviour of the particle system
	Vec3 mPosition;
	Vec3 mAcceleration;
	Vec3 mDirection;
	Vec3 mRandomVelocity;
	float mAgeLimit;
	float mFadeTime;

};

#endif
