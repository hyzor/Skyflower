#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "Sound/SoundEngine.h"
#include "shared/util.h"

#include "Footsteps.h"
#include "Entity.h"
#include "Application.h"

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

static const char *AIrunSounds[] = {
	"AI/cartoonrun1.wav",
	"AI/cartoonrun2.wav",
	"AI/cartoonrun3.wav"
};

FootstepsComponent::FootstepsComponent() : Component("Footsteps")
{
}

FootstepsComponent::~FootstepsComponent()
{
}

void FootstepsComponent::addedToEntity()
{
	m_oldPosition = getOwner()->returnPos();
	m_oldPosition.Y = 0.0f;
	m_distanceTraveled = 0.0f;
}

void FootstepsComponent::update(float deltaTime)
{
	if (getOwner()->getPhysics()->GetStates()->isJumping) {
		return;
	}

	Entity *owner = getOwner();

	Vec3 originalPosition = getOwner()->returnPos();
	Vec3 position = Vec3(originalPosition.X, 0.0f, originalPosition.Z);

	m_distanceTraveled += (position - m_oldPosition).Length();
	m_oldPosition = position;

	if (owner->hasComponents("AI"))
	{
		if (m_distanceTraveled >= 10.0f && owner->getComponent<Movement*>("Movement")->GetSpeed() > 20 && !owner->getPhysics()->GetStates()->isBeingPushed)
		{
			m_distanceTraveled = 0.0f;

			// FIXME: Project the sound's position onto the ground.
			owner->getModules()->sound->PlaySound(AIrunSounds[rand() % ARRAY_SIZE(AIrunSounds)], 3.0f, &originalPosition.X);
		}
	}
	else
	{
		if (m_distanceTraveled >= 13.0f)
		{
			m_distanceTraveled = 0.0f;

			// FIXME: Project the sound's position onto the ground.
			owner->getModules()->sound->PlaySound(GetPlayerSoundFile(footstepSounds[rand() % ARRAY_SIZE(footstepSounds)]), 1.0f, &originalPosition.X);
		}
	}
}
