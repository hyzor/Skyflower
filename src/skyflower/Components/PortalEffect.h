#ifndef COMPONENT_PORTALEFFECT_H
#define COMPONENT_PORTALEFFECT_H

#include "shared/Vec3.h"
#include "graphics/ParticleSystem.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

#define PORTAL_EFFECT_PARTICLE_SYSTEMS 1
#define PORTAL_EFFECT_DURATION         3.0f

class PortalEffectComponent : public Component
{
public:
	PortalEffectComponent();
	virtual ~PortalEffectComponent();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

private:
	ParticleSystem *m_particleSystems[PORTAL_EFFECT_PARTICLE_SYSTEMS];
	float m_animationTime[PORTAL_EFFECT_PARTICLE_SYSTEMS];
	Vec3 m_right;
	Vec3 m_up;
	Vec3 m_forward;
};

#endif
