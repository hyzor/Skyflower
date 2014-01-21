#ifndef COMPONENTS_FOOTSTEPS_H
#define COMPONENTS_FOOTSTEPS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "shared/Vec3.h"
#include "shared/util.h"
#include "Sound/SoundEngine.h"

#include "Cistron.h"
#include "Component.h"
#include "Entity.h"

using namespace Cistron;

static const std::string footstepSounds[] = {
	"player/footstep1.wav",
	"player/footstep2.wav",
	"player/footstep3.wav",
	"player/footstep4.wav",
	"player/footstep5.wav",
	"player/footstep6.wav",
	"player/footstep7.wav",
	"player/footstep8.wav",
	"player/footstep9.wav",
	"player/footstep10.wav",
	"player/footstep11.wav"
};

class FootstepsComponent : public Component
{
public:
	FootstepsComponent() : Component("Footsteps")
	{
	}

	virtual ~FootstepsComponent()
	{
	}

	void addedToEntity()
	{
		m_oldPosition = getEntityPos();
		m_oldPosition.Y = 0.0f;
		m_distanceTraveled = 0.0f;
	}

	void update(float deltaTime)
	{
		if (getOwner()->getPhysics()->isJumping()) {
			return;
		}

		Vec3 originalPosition = getEntityPos();
		Vec3 position = Vec3(originalPosition.X, 0.0f, originalPosition.Z);

		m_distanceTraveled += (position - m_oldPosition).Length();
		m_oldPosition = position;

		if (m_distanceTraveled >= 30.0f) {
			m_distanceTraveled = 0.0f;
			Entity *owner = getOwner();

			if (owner) {
				// FIXME: Project the sound's position onto the ground.
				owner->getModules()->sound->PlaySound(footstepSounds[rand() % ARRAY_SIZE(footstepSounds)], &originalPosition.X, 0.5f);
			}
		}
	}

private:
	Vec3 m_oldPosition;
	float m_distanceTraveled;
};

#endif
