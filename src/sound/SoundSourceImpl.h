#ifndef SOUND_SOUNDSOURCEIMPL_H
#define SOUND_SOUNDSOURCEIMPL_H

#include <cstdint>
#include <string>

#include <AL/al.h>

#include "Sound/SoundSource.h"
#include "ResourceCache.h"

class SoundSourceImpl : public SoundSource
{
public:
	SoundSourceImpl(ResourceCache *resourceCache);
	virtual ~SoundSourceImpl();

	virtual void SetResource(const std::string &name);

	void Update();

	virtual void Play();
	virtual void Pause();

	virtual void Seek(float time);

	virtual bool IsPlaying();
	virtual bool IsLooping();

	virtual void SetVolume(float volume);
	virtual void SetLooping(bool looping);
	virtual void SetPitch(float pitch);

	virtual void SetPosition(const float position[3]);
	virtual void SetVelocity(const float velocity[3]);
	virtual void SetIsRelativeToListener(bool relative);

private:
	void ClearBuffers();

private:
	ResourceCache *m_resourceCache;

	ALuint m_source;
	uint32_t m_resourceHash;
	unsigned int m_nextBufferIndex;
	int m_queuedBufferIndices[2];

	bool m_isLastBufferQueued;
	bool m_isPlaying;
	bool m_isLooping;
};

#endif
