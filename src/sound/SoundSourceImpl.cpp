#include <cassert>
#include <algorithm>
#include <functional>

#include "SoundSourceImpl.h"
#include "SoundEngineImpl.h"
#include "SoundResource.h"
#include "TaskQueue.h"

SoundSourceImpl::SoundSourceImpl(SoundEngineImpl *engine)
{
	m_engine = engine;

	alGenSources(1, &m_source);
	m_resource = NULL;
	m_isPlaying = false;
	m_isLooping = false;
	m_isLoadingBuffer = false;
	m_isLoadingResource = false;
	m_cancelBufferLoading = false;
	m_nextSample = 0;
	m_pendingSeek = -1.0f;
}

SoundSourceImpl::~SoundSourceImpl()
{
	SetResource(NULL);
	alDeleteSources(1, &m_source);
}

void SoundSourceImpl::ReleaseAllQueuedBuffers()
{
	if (!m_resource || !m_resource->IsStreaming())
		return;

	int count = 0;
	ALuint buffer = AL_NONE;

	alSourceStop(m_source);
	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &count);

	for (int i = 0; i < count; i++)
	{
		alSourceUnqueueBuffers(m_source, 1, &buffer);
		m_engine->ReleaseBuffer(buffer);
	}
}

void SoundSourceImpl::SetResource(const char *name)
{
	if (m_isLoadingResource)
	{
		printf("SoundSource::SetResource(\"%s\"), another resource is already being loaded!\n", name);
		return;
	}

	if (name && m_resource && m_resource->GetName() == std::string(name))
		return;

	if (m_isLoadingBuffer)
		m_cancelBufferLoading = true;

	// The source must be stopped before we can change any buffers.
	alSourceStop(m_source);

	if (m_resource)
	{
		ReleaseAllQueuedBuffers();
		alSourcei(m_source, AL_BUFFER, AL_NONE);
		m_engine->ReleaseSoundResource(m_resource);
	}

	m_resource = NULL;

	if (name)
	{
		SoundSourceImpl *source = this;

		std::function<void()> task = [source, name, this]() {
			SoundResource *resource = m_engine->GetSoundResource(name);

			if (resource)
			{
				if (!resource->IsStreaming())
				{
					alSourcei(source->m_source, AL_BUFFER, resource->GetBuffer());
					alSourcei(source->m_source, AL_LOOPING, (source->m_isLooping ? AL_TRUE : AL_FALSE));
				}
				else
				{
					source->m_nextSample = 0;
					alSourcei(source->m_source, AL_LOOPING, AL_FALSE);
				}

				if (source->m_isPlaying)
					source->Play();
			}

			source->m_resource = resource;
			source->m_isLoadingResource = false;

			if (source->m_pendingSeek > 0.0f)
			{
				source->Seek(source->m_pendingSeek);
				source->m_pendingSeek = -1.0f;
			}
		};

		m_isLoadingResource = true;
		m_engine->m_taskQueue->EnqueueTask(task);
	}
}

SoundResource *SoundSourceImpl::GetResource()
{
	return m_resource;
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

	if (m_resource && !m_resource->IsStreaming())
		alSourcei(m_source, AL_LOOPING, (looping ? AL_TRUE : AL_FALSE));
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
