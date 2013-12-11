#include <cassert>
#include <cstdint>

#include "SoundSourceImpl.h"
#include "ResourceCache.h"
#include "OpenALSourceProxy.h"

SoundSourceImpl::SoundSourceImpl(ResourceCache *resourceCache)
{
	m_resourceCache = resourceCache;
	m_resourceHash = 0x0;

	m_nextBufferIndex = 0;
	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;
	m_isLastBufferQueued = false;
	m_pendingSeekSample = -1;
	m_pendingSeekTime = -1.0f;
}

SoundSourceImpl::~SoundSourceImpl()
{
}

void SoundSourceImpl::LendSource(ALuint source)
{
	//printf("Lended source %i\n", source);

	float time = m_sourceProxy.GetPlaybackTime();
	m_sourceProxy.LendSource(source);
	Seek(time);
}

ALuint SoundSourceImpl::RevokeSource()
{
	ClearBuffers();

	ALuint source = m_sourceProxy.RevokeSource();
	
	if (m_pendingSeekSample >= 0) {
		//printf("SoundSourceImpl::RevokeSource, seeking to %f\n", m_pendingSeekTime);

		m_sourceProxy.Seek(m_pendingSeekTime, (uint64_t)m_pendingSeekSample);
		m_pendingSeekSample = -1;
		m_pendingSeekTime = -1.0f;
	}

	//printf("Revoked source %i\n", source);

	return source;
}

bool SoundSourceImpl::HasSource() const
{
	return m_sourceProxy.HasSource();
}

void SoundSourceImpl::ClearBuffers()
{
	m_sourceProxy.ClearBuffers();

	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;
	m_isLastBufferQueued = false;
}

void SoundSourceImpl::SetResource(const std::string &name)
{
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

void SoundSourceImpl::Update(float deltaTime)
{
	if (m_resourceHash == 0x0) {
		return;
	}

	bool isStreaming = m_resourceCache->IsResourceStreaming(m_resourceHash);

	if (m_pendingSeekSample >= 0) {
		bool shouldSeek = true;

		if (m_sourceProxy.HasSource()) {
			if (!isStreaming && m_sourceProxy.GetBuffer() == AL_NONE) {
				shouldSeek = false;
			}
			else if (isStreaming && m_sourceProxy.GetNumQueuedBuffers() == 0) {
				shouldSeek = false;
			}
		}

		if (shouldSeek) {
			//printf("Seeking to time=%f\n", m_pendingSeekTime);

			m_sourceProxy.Seek(m_pendingSeekTime, (uint64_t)m_pendingSeekSample);
			m_pendingSeekSample = -1;
			m_pendingSeekTime = -1.0f;
		}
	}

	m_sourceProxy.Update(deltaTime);

	if (!m_sourceProxy.HasSource()) {
		return;
	}

	bool needBuffer = false;

	if (!isStreaming) {
		if (m_sourceProxy.GetBuffer() == AL_NONE) {
			needBuffer = true;
		}

		// Touch the buffer so it won't get thrown out of the cache.
		m_resourceCache->RequestBuffer(m_resourceHash, 0, NULL);
	}
	else {
		unsigned int numProcessedBuffers = m_sourceProxy.UnqueueProcessedBuffers();
		unsigned int lastBufferIndex = m_resourceCache->GetResourceBufferCount(m_resourceHash) - 1;

		if (numProcessedBuffers > 0) {
			// FIXME: Move this to OpenALSourceProxy? (send the bufferIndex with QueueBuffer and the resourceHash(lastBufferIndex) somewhere?)
			// What benefit would it bring to move it to OpenALSourceProxy?
			for (unsigned int i = 0; i < numProcessedBuffers; i++) {
				if (m_queuedBufferIndices[i] == lastBufferIndex) {
					m_isLastBufferQueued = false;

					if (!IsLooping() && IsPlaying()) {
						// We reached the end of a non-looping source, stop playback.
						Pause();

						printf("We just unqueued the last buffer of a non-looping source, stopping\n");
					}
				}
			}
			
			// Move the remaining queued buffers to the start of the array.
			for (int i = numProcessedBuffers; i < 2; i++) {
				m_queuedBufferIndices[i - numProcessedBuffers] = m_queuedBufferIndices[i];
			}
			// Clear the end of the array.
			for (int i = 2 - numProcessedBuffers; i < 2; i++) {
				m_queuedBufferIndices[i] = -1;
			}
		}

		if (m_sourceProxy.GetNumQueuedBuffers() < 2) {
			// FIXME: Also start loading the second buffer if numQueuedBuffers is zero.
			needBuffer = true;
		}

		// Touch the buffers so they won't get thrown out of the cache.
		for (int i = 0; i < 2; i++) {
			if (m_queuedBufferIndices[i] != -1) {
				m_resourceCache->RequestBuffer(m_resourceHash, m_queuedBufferIndices[i], NULL);
			}
		}
	}

	if (needBuffer) {
		ALuint buffer;

		switch (m_resourceCache->RequestBuffer(m_resourceHash, m_nextBufferIndex, &buffer)) {
		case BufferStatusLoaded:
			if (!isStreaming) {
				m_sourceProxy.SetBuffer(buffer);
			}
			else {
				if (!IsLooping() && m_isLastBufferQueued) {
					break;
				}

				m_sourceProxy.QueueBuffer(buffer);

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

			break;
		case BufferStatusLoadingHeaders:
			//printf("SoundSourceImpl::Update, BufferStatusLoadingHeaders\n");
			break;
		case BufferStatusPendingLoad:
			//printf("SoundSourceImpl::Update, BufferStatusPendingLoad\n");
			break;
		case BufferStatusIndexOutOfBounds:
			printf("SoundSourceImpl::Update, BufferStatusIndexOutOfBounds, maxIndex=%i, index=%i\n", m_resourceCache->GetResourceBufferCount(m_resourceHash), m_nextBufferIndex);
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
	m_sourceProxy.Play();
}

void SoundSourceImpl::Pause()
{
	m_sourceProxy.Pause();
}

void SoundSourceImpl::Seek(float time)
{
	assert(time >= 0.0f);

	if (m_resourceHash == 0x0) {
		return;
	}

	uint64_t sampleOffset;
	unsigned int bufferIndex = m_resourceCache->ConvertTimeToBufferIndex(m_resourceHash, time, &sampleOffset);

	m_pendingSeekSample = sampleOffset;
	m_pendingSeekTime = time;

	//printf("Queueing seek: time=%f, bufferIndex=%i, sampleOffset=%i\n", time, bufferIndex, sampleOffset);

	if (m_resourceCache->IsResourceStreaming(m_resourceHash) && bufferIndex != m_queuedBufferIndices[0]) {
		ClearBuffers();
		m_nextBufferIndex = bufferIndex;
	}
}

bool SoundSourceImpl::IsPlaying() const
{
	return m_sourceProxy.IsPlaying();
}

bool SoundSourceImpl::IsLooping() const
{
	return m_sourceProxy.IsLooping();
}

void SoundSourceImpl::SetVolume(float volume)
{
	assert(volume >= 0.0f);

	m_sourceProxy.SetVolume(volume);
}

void SoundSourceImpl::SetLooping(bool looping)
{
	m_sourceProxy.SetLooping(looping);
}

void SoundSourceImpl::SetPitch(float pitch)
{
	assert(pitch >= 0.0f);

	m_sourceProxy.SetPitch(pitch);
}

void SoundSourceImpl::SetPosition(const float position[3])
{
	m_sourceProxy.SetPosition(position);
}

void SoundSourceImpl::SetVelocity(const float velocity[3])
{
	m_sourceProxy.SetVelocity(velocity);
}

void SoundSourceImpl::SetIsRelativeToListener(bool relative)
{
	m_sourceProxy.SetRelativeToListener(relative);
}

void SoundSourceImpl::GetPosition(float output[3]) const
{
	m_sourceProxy.GetPosition(output);
}
