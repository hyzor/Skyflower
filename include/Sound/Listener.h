#ifndef SOUND_LISTENER_H
#define SOUND_LISTENER_H

#include "shared/platform.h"

class DLL_API Listener
{
public:
	virtual ~Listener() {}

	virtual void SetMasterVolume(float volume) = 0;

	virtual void SetPosition(const float position[3]) = 0;
	virtual void SetOrientation(const float forward[3], const float up[3]) = 0;

	// Used for the doppler effect, should be in the same unit as the speed of sound set in SoundEngine.
	virtual void SetVelocity(const float velocity[3]) = 0;
};

#endif
