#ifndef SOUND_SOUNDENGINE_H
#define SOUND_SOUNDENGINE_H

#ifdef _WIN32
	#ifdef COMPILING_DLL
		#define DLL_EXPORT __declspec(dllexport)
	#else
		#define DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define DLL_EXPORT
#endif

#include <string>

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

	virtual void PlaySound(const std::string &file, const float position[3], float volume, bool relativeToListener = false) = 0;

	virtual void SetActiveListener(Listener *listener) = 0;
	virtual void SetDopplerFactor(float dopplerFactor) = 0;
	// Used for the doppler effect, default value is 343.3 (Sound travels through air at ~343m/s)
	virtual void SetSpeedOfSound(float speed) = 0;

	virtual void Update(float deltaTime) = 0;
};

DLL_EXPORT SoundEngine *CreateSoundEngine(const std::string &resourceDir);
DLL_EXPORT void DestroySoundEngine(SoundEngine *engine);

#endif
