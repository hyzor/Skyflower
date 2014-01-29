#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "Sound/SoundEngine.h"
#include "shared/util.h"

#include "Footsteps.h"
#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

static const char *footstepSounds[] = {
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

FootstepsComponent::FootstepsComponent() : Component("Footsteps")
{
}

FootstepsComponent::~FootstepsComponent()
{
}

void FootstepsComponent::addedToEntity()
{
	m_oldPosition = getEntityPos();
	m_oldPosition.Y = 0.0f;
	m_distanceTraveled = 0.0f;
}

void FootstepsComponent::update(float deltaTime)
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
