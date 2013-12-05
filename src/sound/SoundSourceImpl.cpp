#include <cassert>
#include <cstdint>

#include "SoundSourceImpl.h"
#include "ResourceCache.h"

SoundSourceImpl::SoundSourceImpl(ResourceCache *resourceCache)
{
	m_resourceCache = resourceCache;

	alGenSources(1, &m_source);
	m_resourceHash = 0x0;

	m_isPlaying = false;
	m_isLooping = false;
}

SoundSourceImpl::~SoundSourceImpl()
{
	ClearBuffers();

	alDeleteSources(1, &m_source);
}

void SoundSourceImpl::ClearBuffers()
{
	int count = 0;
	ALuint buffer = AL_NONE;

	// The source must be stopped before we can change any buffers.
	alSourceStop(m_source);

	alSourcei(m_source, AL_BUFFER, AL_NONE);
	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &count);

	for (int i = 0; i < count; i++) {
		alSourceUnqueueBuffers(m_source, 1, &buffer);
	}
}

void SoundSourceImpl::SetResource(const std::string &name)
{
	// The source must be stopped before we can change any buffers.
	alSourceStop(m_source);
	// Unload any previous buffers.
	ClearBuffers();

	m_resourceHash = m_resourceCache->PrefetchResource(name);

	ALuint buffer;

	if (m_resourceCache->RequestBuffer(m_resourceHash, 0, &buffer) == BufferStatusLoaded) {
		if (!m_resourceCache->IsResourceStreaming(m_resourceHash)) {
			alSourcei(m_source, AL_BUFFER, buffer);
			alSourcei(m_source, AL_LOOPING, (m_isLooping ? AL_TRUE : AL_FALSE));
		}
		else {
			//source->m_nextSample = 0;
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		if (m_isPlaying) {
			Play();
		}
	}
}

void SoundSourceImpl::Update()
{
	if (m_resourceHash == 0x0) {
		return;
	}

	bool isStreaming = m_resourceCache->IsResourceStreaming(m_resourceHash);
	bool needBuffer = false;

	if (!isStreaming) {
		ALint sourceState, attachedBuffer;
		alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
		alGetSourcei(m_source, AL_BUFFER, &attachedBuffer);

		if (attachedBuffer == AL_NONE) {
			needBuffer = true;
		}
		else {
			// Touch the buffer so it won't get thrown out of the cache.
			m_resourceCache->RequestBuffer(m_resourceHash, 0, NULL);

			if (m_isPlaying && sourceState != AL_PLAYING) {
				m_isPlaying = false;

				printf("Reached end of non-streaming source\n");
			}
		}
	}
	else {
		ALint processedBuffers, queuedBuffers;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBuffers);

		assert(processedBuffers <= 2);

		alSourceUnqueueBuffers(m_source, processedBuffers, NULL);

		if (queuedBuffers < 2) {
			needBuffer = true;
		}

		for (int i = 0; i < queuedBuffers; i++) {
			// FIXME: Touch buffer!
		}

		// FIXME: Detect if we reached the end and check m_isLooping.
		// We should still load and attach buffer 0 and 1, but we should stop playing.
	}

	if (needBuffer) {
		ALuint buffer;

		switch (m_resourceCache->RequestBuffer(m_resourceHash, 0/*m_nextBufferIndex*/, &buffer)) {
		case BufferStatusLoaded:
			if (!isStreaming) {
				// The source must be stopped before we can change any buffers.
				alSourceStop(m_source);
				alSourcei(m_source, AL_BUFFER, buffer);
			}
			else {
				alSourceQueueBuffers(m_source, 1, &buffer);
				// m_nextBufferIndex++;
			}

			if (m_isPlaying) {
				Play();
			}

			break;
		case BufferStatusIndexOutOfBounds:
			// FIXME: We can only set m_nextBufferIndex to zero here if we are looping.
			// Otherwise we might start playing from the start again if we can't detect we
			// played to the end in time.

			// m_nextBufferIndex = 0;
			break;
		case BufferStatusLoadingHeaders:
		case BufferStatusPendingLoad:
		case BufferStatusOutOfMemory:
			break;
		case BufferStatusInvalidFile:
			m_resourceHash = 0x0;
			return;
		}
	}
}

uint32_t SoundSourceImpl::GetResourceHash()
{
	return m_resourceHash;
}

void SoundSourceImpl::Play()
{
	alSourcePlay(m_source);
	m_isPlaying = true;
}

void SoundSourceImpl::Pause()
{
	alSourcePause(m_source);
	m_isPlaying = false;
}

void SoundSourceImpl::Seek(float time)
{
#if 0
	if (m_isLoadingResource)
	{
		m_pendingSeek = time;
		return;
	}

	if (!m_resource)
		return;

	size_t sample = std::min(m_resource->GetSampleCountForTime(std::max(time, 0.0f)), m_resource->GetTotalSampleCount() - 1);

	if (!m_resource->IsStreaming())
	{
		alSourcei(m_source, AL_SAMPLE_OFFSET, (ALint)sample);
	}
	else
	{
		if (m_isLoadingBuffer)
			m_cancelBufferLoading = true;

		ReleaseAllQueuedBuffers();
		m_nextSample = sample;
	}
#endif
}

bool SoundSourceImpl::IsPlaying()
{
	return m_isPlaying;
}

bool SoundSourceImpl::IsLooping()
{
	return m_isLooping;
}

void SoundSourceImpl::SetVolume(float volume)
{
	alSourcef(m_source, AL_GAIN, volume);
}

void SoundSourceImpl::SetLooping(bool looping)
{
	m_isLooping = looping;

	if (m_resourceHash && !m_resourceCache->IsResourceStreaming(m_resourceHash)) {
		alSourcei(m_source, AL_LOOPING, (looping? AL_TRUE : AL_FALSE));
	}
}

void SoundSourceImpl::SetPitch(float pitch)
{
	alSourcef(m_source, AL_PITCH, pitch);
}

void SoundSourceImpl::SetPosition(const float position[3])
{
	alSourcefv(m_source, AL_POSITION, position);
}

void SoundSourceImpl::SetVelocity(const float velocity[3])
{
	alSourcefv(m_source, AL_VELOCITY, velocity);
}

void SoundSourceImpl::SetIsRelativeToListener(bool relative)
{
	alSourcei(m_source, AL_SOURCE_RELATIVE, (relative ? AL_TRUE : AL_FALSE));
}
