#ifndef SOUND_SOUNDENGINE_H
#define SOUND_SOUNDENGINE_H

#include <string>

#include "shared/platform.h"

#include "Sound/Listener.h"
#include "Sound/SoundSource.h"

class SoundEngine
{
public:
	virtual ~SoundEngine() {}

	virtual SoundSource *CreateSource() = 0;
	virtual Listener *CreateListener() = 0;

	virtual void DestroySource(SoundSource *source) = 0;
	virtual void DestroyListener(Listener *listener) = 0;

	virtual void PlaySound(const char *file, const float position[3], float volume, bool relativeToListener = false) = 0;

	virtual void SetActiveListener(Listener *listener) = 0;
	virtual void SetDopplerFactor(float dopplerFactor) = 0;
	// Used for the doppler effect, default value is 343.3 (Sound travels through air at ~343m/s)
	virtual void SetSpeedOfSound(float speed) = 0;

	virtual void Update(float deltaTime) = 0;
};

DLL_API SoundEngine *CreateSoundEngine(const std::string &resourceDir);
DLL_API void DestroySoundEngine(SoundEngine *engine);

#endif
