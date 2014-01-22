#include <cassert>
#include <cstdint>

#include "SoundSourceImpl.h"
#include "ResourceCache.h"
#include "OpenALSourceProxy.h"

SoundSourceImpl::SoundSourceImpl(ResourceCache *resourceCache) :
	m_sourceProxy(this)
{
	m_resourceCache = resourceCache;
	m_resourceHash = 0x0;

	m_nextBufferIndex = 0;
	m_pendingSeekSample = -1;
	m_pendingSeekTime = -1.0f;

	m_playbackFinishedHandler = []() {};
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

void SoundSourceImpl::SetResource(const std::string &name)
{
	m_sourceProxy.ClearBuffers();

	m_resourceHash = m_resourceCache->PrefetchResource(name);
	m_nextBufferIndex = 0;

	// Start loading the first buffer.
	m_resourceCache->RequestBuffer(m_resourceHash, 0, NULL);

	// If we are streaming, also start loading the second buffer.
	if (m_resourceCache->IsResourceStreaming(m_resourceHash)) {
		m_resourceCache->RequestBuffer(m_resourceHash, 1, NULL);
	}
}

const struct AudioResourceInfo *SoundSourceImpl::GetResourceInfo() const
{
	return m_resourceCache->GetResourceInfo(m_resourceHash);
}

void SoundSourceImpl::Update(float deltaTime)
{
	if (m_resourceHash == 0x0) {
		return;
	}

	const struct AudioResourceInfo *resourceInfo = m_resourceCache->GetResourceInfo(m_resourceHash);

	if (!resourceInfo) {
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
		m_sourceProxy.UnqueueProcessedBuffers();
		
		if (m_sourceProxy.GetNumQueuedBuffers() < 2) {
			// FIXME: Also start loading the second buffer if numQueuedBuffers is zero.
			needBuffer = true;
		}

		int queuedBufferIndices[2];
		m_sourceProxy.GetQueuedBufferIndices(queuedBufferIndices);

		// Touch the buffers so they won't get thrown out of the cache.
		for (int i = 0; i < 2; i++) {
			if (queuedBufferIndices[i] != -1) {
				m_resourceCache->RequestBuffer(m_resourceHash, queuedBufferIndices[i], NULL);
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
				if (!IsLooping() && m_sourceProxy.IsLastBufferQueued()) {
					break;
				}

				m_sourceProxy.QueueBuffer(buffer, m_nextBufferIndex);
				m_nextBufferIndex = (m_nextBufferIndex + 1) % resourceInfo->bufferCount;
			}

			break;
		case BufferStatusLoadingHeaders:
			//printf("SoundSourceImpl::Update, BufferStatusLoadingHeaders\n");
			break;
		case BufferStatusPendingLoad:
			//printf("SoundSourceImpl::Update, BufferStatusPendingLoad\n");
			break;
		case BufferStatusIndexOutOfBounds:
			printf("SoundSourceImpl::Update, BufferStatusIndexOutOfBounds, maxIndex=%i, index=%i\n", resourceInfo->bufferCount, m_nextBufferIndex);
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

	int queuedBufferIndices[2];
	m_sourceProxy.GetQueuedBufferIndices(queuedBufferIndices);

	if (m_resourceCache->IsResourceStreaming(m_resourceHash) && bufferIndex != queuedBufferIndices[0]) {
		m_sourceProxy.ClearBuffers();
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

bool SoundSourceImpl::IsRelativeToListener() const
{
	return m_sourceProxy.IsRelativeToListener();
}

void SoundSourceImpl::SetPlaybackFinishedHandler(const std::function<void()> &handler)
{
	m_playbackFinishedHandler = handler;
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

void SoundSourceImpl::SetRelativeToListener(bool relative)
{
	m_sourceProxy.SetRelativeToListener(relative);
}

void SoundSourceImpl::GetPosition(float output[3]) const
{
	m_sourceProxy.GetPosition(output);
}

void SoundSourceImpl::OnPlaybackFinished() const
{
	m_playbackFinishedHandler();
}
