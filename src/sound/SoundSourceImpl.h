#ifndef SOUND_SOUNDSOURCEIMPL_H
#define SOUND_SOUNDSOURCEIMPL_H

#include <cstdint>
#include <string>

#include <AL/al.h>

#include "Sound/SoundSource.h"
#include "AudioResource.h"
#include "ResourceCache.h"
#include "OpenALSourceProxy.h"

class SoundSourceImpl : public SoundSource
{
public:
	SoundSourceImpl(ResourceCache *resourceCache);
	virtual ~SoundSourceImpl();

	void LendSource(ALuint source);
	ALuint RevokeSource();
	bool HasSource() const;

	virtual void SetResource(const std::string &name);
	const struct AudioResourceInfo *GetResourceInfo() const;

	void Update(float deltaTime);

	virtual void Play();
	virtual void Pause();

	virtual void Seek(float time);

	virtual bool IsPlaying() const;
	virtual bool IsLooping() const;
	virtual bool IsRelativeToListener() const;

	virtual void SetVolume(float volume);
	virtual void SetLooping(bool looping);
	virtual void SetPitch(float pitch);

	virtual void SetPosition(const float position[3]);
	virtual void SetVelocity(const float velocity[3]);
	virtual void SetRelativeToListener(bool relative);

	virtual void GetPosition(float output[3]) const;

private:
	void ClearBuffers();

private:
	ResourceCache *m_resourceCache;
	uint32_t m_resourceHash;
	OpenALSourceProxy m_sourceProxy;

	unsigned int m_nextBufferIndex;
	// FIXME: Replace with m_sourceProxy->GetQueuedBufferIndices?
	int m_queuedBufferIndices[2];
	bool m_isLastBufferQueued;
	int64_t m_pendingSeekSample;
	float m_pendingSeekTime;
};

#endif
