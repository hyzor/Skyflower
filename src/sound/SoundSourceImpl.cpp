#include <cassert>
#include <cstdint>

#include "SoundSourceImpl.h"
#include "ResourceCache.h"

SoundSourceImpl::SoundSourceImpl(ResourceCache *resourceCache)
{
	m_resourceCache = resourceCache;

	alGenSources(1, &m_source);
	m_resourceHash = 0x0;
	m_nextBufferIndex = 0;
	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;

	m_isLastBufferQueued = false;
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

	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;

	m_isLastBufferQueued = false;
}

void SoundSourceImpl::SetResource(const std::string &name)
{
	// The source must be stopped before we can change any buffers.
	alSourceStop(m_source);
	// Unload any previous buffers.
	ClearBuffers();

	m_resourceHash = m_resourceCache->PrefetchResource(name);
	m_nextBufferIndex = 0;

	// Start loading the first buffer.
	m_resourceCache->RequestBuffer(m_resourceHash, 0, NULL);

	// If we are streaming, also start loading the second buffer.
	if (m_resourceCache->IsResourceStreaming(m_resourceHash)) {
		m_resourceCache->RequestBuffer(m_resourceHash, 1, NULL);
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

				//printf("Reached end of non-streaming non-looping source, stopping\n");
			}
		}
	}
	else {
		ALint processedBuffers, queuedBuffers;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBuffers);

		assert(processedBuffers <= 2);

		ALuint unqueuedBuffers[2];
		alSourceUnqueueBuffers(m_source, processedBuffers, unqueuedBuffers);

		unsigned int lastBufferIndex = m_resourceCache->GetResourceBufferCount(m_resourceHash) - 1;

		if (processedBuffers > 0) {
			for (int i = 0; i < processedBuffers; i++) {
				if (m_queuedBufferIndices[i] == lastBufferIndex) {
					m_isLastBufferQueued = false;

					if (!m_isLooping && m_isPlaying) {
						// We reached the end of a non-looping source, stop playback.
						m_isPlaying = false;

						//printf("We just unqueued the last buffer of a non-looping source, stopping\n");
					}
				}
			}
			
			// Move the remaining queued buffers to the start of the array.
			for (int i = processedBuffers; i < 2; i++) {
				m_queuedBufferIndices[i - processedBuffers] = m_queuedBufferIndices[i];
			}
			// Clear the end of the array.
			for (int i = 2 - processedBuffers; i < 2; i++) {
				m_queuedBufferIndices[i] = -1;
			}
		}

		if (queuedBuffers < 2) {
			needBuffer = true;
		}

		// Touch the queued buffers so they won't get thrown out of the cache.
		for (int i = 0; i < queuedBuffers; i++) {
			m_resourceCache->RequestBuffer(m_resourceHash, m_queuedBufferIndices[i], NULL);
		}
	}

	if (needBuffer) {
		ALuint buffer;

		switch (m_resourceCache->RequestBuffer(m_resourceHash, m_nextBufferIndex, &buffer)) {
		case BufferStatusLoaded:
			alSourcei(m_source, AL_LOOPING, ((m_isLooping && !isStreaming)? AL_TRUE : AL_FALSE));

			if (!isStreaming) {
				// The source must be stopped before we can change any buffers.
				alSourceStop(m_source);
				alSourcei(m_source, AL_BUFFER, buffer);
			}
			else {
				if (!m_isLooping && m_isLastBufferQueued) {
					break;
				}

				alSourceQueueBuffers(m_source, 1, &buffer);

				for (int i = 0; i < 2; i++) {
					if (m_queuedBufferIndices[i] == -1) {
						m_queuedBufferIndices[i] = m_nextBufferIndex;
						break;
					}
				}

				m_nextBufferIndex++;

				if (m_nextBufferIndex == m_resourceCache->GetResourceBufferCount(m_resourceHash)) {
					m_isLastBufferQueued = true;
					m_nextBufferIndex = 0;
				}
			}

			if (m_isPlaying) {
				Play();
			}

			break;
		case BufferStatusLoadingHeaders:
			//printf("SoundSourceImpl::Update, BufferStatusLoadingHeaders\n");
			break;
		case BufferStatusPendingLoad:
			//printf("SoundSourceImpl::Update, BufferStatusPendingLoad\n");
			break;
		case BufferStatusIndexOutOfBounds:
			printf("SoundSourceImpl::Update, BufferStatusIndexOutOfBounds\n");
			break;
		case BufferStatusOutOfMemory:
			printf("SoundSourceImpl::Update, BufferStatusOutOfMemory\n");
			break;
		case BufferStatusInvalidFile:
			printf("SoundSourceImpl::Update, BufferStatusInvalidFile\n");
			m_resourceHash = 0x0;
			return;
		}
	}
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
	// FIXME: Implement!
	assert(0);

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
