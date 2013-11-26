#ifndef SOUND_SOUNDSOURCEIMPL_H
#define SOUND_SOUNDSOURCEIMPL_H

#include <string>

#include <AL/al.h>

#include "Sound/SoundSource.h"
#include "SoundEngineImpl.h"

class SoundResource;
class SoundEngineImpl;

class SoundSourceImpl : public SoundSource
{
public:
	SoundSourceImpl(SoundEngineImpl *engine);
	virtual ~SoundSourceImpl();

	void SetResource(const char *name);
	SoundResource *GetResource();

	void Play();
	void Pause();

	void Seek(float time);

	bool IsPlaying();
	bool IsLooping();

	void SetVolume(float volume);
	void SetLooping(bool looping);
	void SetPitch(float pitch);

	void SetPosition(const float position[3]);
	void SetVelocity(const float velocity[3]);
	void SetIsRelativeToListener(bool relative);

private:
	friend class SoundEngineImpl;

	void ReleaseAllQueuedBuffers();

private:
	SoundEngineImpl *m_engine;

	ALuint m_source;
	SoundResource *m_resource;

	bool m_isPlaying;
	bool m_isLooping;

	bool m_isLoadingBuffer;
	bool m_isLoadingResource;
	bool m_cancelBufferLoading;

	size_t m_nextSample;
	float m_pendingSeek;
};

#endif
