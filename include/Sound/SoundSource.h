#ifndef SOUND_SOUNDSOURCE_H
#define SOUND_SOUNDSOURCE_H

#include <functional>
#include <string>

class SoundSource
{
public:
	virtual ~SoundSource() {}

	virtual void SetResource(const std::string &name) = 0;

	virtual void Play() = 0;
	virtual void Pause() = 0;

	// time is in seconds.
	virtual void Seek(float time) = 0;

	virtual bool IsPlaying() const = 0;
	virtual bool IsLooping() const = 0;
	virtual bool IsRelativeToListener() const = 0;

	virtual void SetPlaybackFinishedHandler(const std::function<void()> &handler) = 0;

	virtual void SetVolume(float volume) = 0;
	virtual void SetLooping(bool looping) = 0;
	virtual void SetPitch(float pitch) = 0;

	virtual void SetPosition(const float position[3]) = 0;
	// Used for the doppler effect, should be in the same unit as the speed of sound set in SoundEngine.
	virtual void SetVelocity(const float velocity[3]) = 0;
	// Sets whether position and velocity should be relative to the listener or not.
	virtual void SetRelativeToListener(bool relative) = 0;

	virtual void GetPosition(float output[3]) const = 0;
};

#endif
