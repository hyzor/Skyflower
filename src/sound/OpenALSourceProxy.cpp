#include <cassert>
#include <cstdio>

#include <AL/al.h>

#include "OpenALSourceProxy.h"
#include "SoundSourceImpl.h"

OpenALSourceProxy::OpenALSourceProxy(const SoundSourceImpl *owner)
{
	assert(owner);

	m_owner = owner;
	m_source = 0;

	m_buffer = AL_NONE;
	m_isPlaying = false;
	m_isLooping = false;
	m_isRelativeToListener = false;
	m_volume = 1.0f;
	m_pitch = 1.0f;

	m_position[0] = 0.0f;
	m_position[1] = 0.0f;
	m_position[2] = 0.0f;

	m_velocity[0] = 0.0f;
	m_velocity[1] = 0.0f;
	m_velocity[2] = 0.0f;

	m_time = 0.0f;
	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;
	m_isLastBufferQueued = false;
}

OpenALSourceProxy::~OpenALSourceProxy()
{
	// Make sure the source has been revoked so we aren't leaking it.
	assert(!HasSource());
}

void OpenALSourceProxy::LendSource(ALuint source)
{
	assert(!HasSource());
	assert(source != 0);
	assert(alIsSource(source));

	m_source = source;
	ClearBuffers();

	// Looping will be set when we first set/queue a buffer.
	alSourcei(m_source, AL_SOURCE_RELATIVE, (m_isRelativeToListener ? AL_TRUE : AL_FALSE));
	alSourcef(m_source, AL_GAIN, m_volume);
	alSourcef(m_source, AL_PITCH, m_pitch);
	alSourcefv(m_source, AL_POSITION, m_position);
	alSourcefv(m_source, AL_VELOCITY, m_velocity);

	assert(alGetError() == AL_NO_ERROR);
}

ALuint OpenALSourceProxy::RevokeSource()
{
	assert(HasSource());
	assert(alIsSource(m_source));

	alGetSourcef(m_source, AL_SEC_OFFSET, &m_time);

	if (m_buffer == AL_NONE) {
		const struct AudioResourceInfo *info = m_owner->GetResourceInfo();

		if (info) {
			uint64_t sampleOffset = m_queuedBufferIndices[0] * info->samplesPerBuffer;
			m_time += (float)sampleOffset / (info->sampleRate * info->channels);
		}
	}

	assert(alGetError() == AL_NO_ERROR);

	ClearBuffers();
	ALuint source = m_source;
	m_source = 0;

	return source;
}

bool OpenALSourceProxy::HasSource() const
{
	return (m_source != 0);
}

void OpenALSourceProxy::ClearBuffers()
{
	if (HasSource()) {
		// The source must be stopped before we can change any buffers.
		alSourceStop(m_source);

		// Setting AL_BUFFER also removes any queued buffers.
		alSourcei(m_source, AL_BUFFER, AL_NONE);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_buffer = AL_NONE;

	m_queuedBufferIndices[0] = -1;
	m_queuedBufferIndices[1] = -1;
	m_isLastBufferQueued = false;
}

void OpenALSourceProxy::Update(float deltaTime)
{
	const struct AudioResourceInfo *info = m_owner->GetResourceInfo();

	if (!info) {
		return;
	}

	if (HasSource() && m_buffer != AL_NONE && m_isPlaying) {
		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING) {
			m_isPlaying = false;

			//printf("Reached end of non-streaming non-looping source, stopping\n");
		}

		assert(alGetError() == AL_NO_ERROR);
	}
	else if (!HasSource() && m_isPlaying) {
		m_time += deltaTime;

		if (m_time >= info->duration) {
			m_time = 0.0f;

			if (!m_isLooping) {
				Pause();
			}
		}
	}
}

void OpenALSourceProxy::Play()
{
	if (HasSource()) {
		alSourcePlay(m_source);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_isPlaying = true;
}

void OpenALSourceProxy::Pause()
{
	if (HasSource()) {
		alSourcePause(m_source);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_isPlaying = false;
}

void OpenALSourceProxy::Seek(float time, uint64_t sample)
{
	if (HasSource()) {
		alSourcei(m_source, AL_SAMPLE_OFFSET, (ALint)sample);

		assert(alGetError() == AL_NO_ERROR);
	}
	else {
		m_time = time;
		//printf("seek without source, time=%i\n", m_time);
	}
}

void OpenALSourceProxy::SetBuffer(ALuint buffer)
{
	assert(HasSource());

	// The source must be stopped before we can change any buffers.
	alSourceStop(m_source);
	alSourcei(m_source, AL_BUFFER, buffer);
	alSourcei(m_source, AL_LOOPING, (m_isLooping? AL_TRUE : AL_FALSE));

	if (m_isPlaying) {
		alSourcePlay(m_source);
	}

	m_buffer = buffer;

	assert(alGetError() == AL_NO_ERROR);
}

void OpenALSourceProxy::SetLooping(bool looping)
{
	if (HasSource()) {
		alSourcei(m_source, AL_LOOPING, ((m_buffer != AL_NONE && looping)? AL_TRUE : AL_FALSE));

		assert(alGetError() == AL_NO_ERROR);
	}

	m_isLooping = looping;
}

void OpenALSourceProxy::SetRelativeToListener(bool relative)
{
	if (HasSource()) {
		alSourcei(m_source, AL_SOURCE_RELATIVE, (relative ? AL_TRUE : AL_FALSE));

		assert(alGetError() == AL_NO_ERROR);
	}

	m_isRelativeToListener = relative;
}

void OpenALSourceProxy::SetVolume(float volume)
{
	if (HasSource()) {
		alSourcef(m_source, AL_GAIN, volume);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_volume = volume;
}

void OpenALSourceProxy::SetPitch(float pitch)
{
	if (HasSource()) {
		alSourcef(m_source, AL_PITCH, pitch);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_pitch = pitch;
}

void OpenALSourceProxy::SetPosition(const float position[3])
{
	if (HasSource()) {
		alSourcefv(m_source, AL_POSITION, position);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_position[0] = position[0];
	m_position[1] = position[1];
	m_position[2] = position[2];
}

void OpenALSourceProxy::SetVelocity(const float velocity[3])
{
	if (HasSource()) {
		alSourcefv(m_source, AL_VELOCITY, velocity);

		assert(alGetError() == AL_NO_ERROR);
	}

	m_velocity[0] = velocity[0];
	m_velocity[1] = velocity[1];
	m_velocity[2] = velocity[2];
}

ALuint OpenALSourceProxy::GetBuffer() const
{
	return m_buffer;
}

unsigned int OpenALSourceProxy::GetNumQueuedBuffers() const
{
	ALint count = 0;

	if (HasSource()) {
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &count);

		assert(alGetError() == AL_NO_ERROR);
	}

	return (unsigned int)count;
}

void OpenALSourceProxy::GetQueuedBufferIndices(int queuedBufferIndices_out[2]) const
{
	queuedBufferIndices_out[0] = m_queuedBufferIndices[0];
	queuedBufferIndices_out[1] = m_queuedBufferIndices[1];
}

bool OpenALSourceProxy::IsPlaying() const
{
	return m_isPlaying;
}

bool OpenALSourceProxy::IsLooping() const
{
	return m_isLooping;
}

bool OpenALSourceProxy::IsRelativeToListener() const
{
	return m_isRelativeToListener;
}

float OpenALSourceProxy::GetVolume() const
{
	return m_volume;
}

float OpenALSourceProxy::GetPitch() const
{
	return m_pitch;
}

void OpenALSourceProxy::GetPosition(float output[3]) const
{
	output[0] = m_position[0];
	output[1] = m_position[1];
	output[2] = m_position[2];
}

void OpenALSourceProxy::GetVelocity(float output[3]) const
{
	output[0] = m_velocity[0];
	output[1] = m_velocity[1];
	output[2] = m_velocity[2];
}

float OpenALSourceProxy::GetPlaybackTime() const
{
	assert(!HasSource());

	return m_time;
}

bool OpenALSourceProxy::IsLastBufferQueued() const
{
	return m_isLastBufferQueued;
}

void OpenALSourceProxy::QueueBuffer(ALuint buffer, unsigned int bufferIndex)
{
	assert(HasSource());
	assert(GetNumQueuedBuffers() < 2);

	alSourceQueueBuffers(m_source, 1, &buffer);
	alSourcei(m_source, AL_LOOPING, AL_FALSE);
	
	if (m_isPlaying) {
		alSourcePlay(m_source);
	}

	for (int i = 0; i < 2; i++) {
		if (m_queuedBufferIndices[i] == -1) {
			m_queuedBufferIndices[i] = bufferIndex;
			break;
		}
	}

	const struct AudioResourceInfo *info = m_owner->GetResourceInfo();

	if (info && bufferIndex == info->bufferCount - 1) {
		m_isLastBufferQueued = true;
	}

	assert(alGetError() == AL_NO_ERROR);
}

unsigned int OpenALSourceProxy::UnqueueProcessedBuffers()
{
	ALint count = 0;

	if (HasSource()) {
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &count);

		assert(count <= 2);

		if (count > 0) {
			ALuint buffers[2];
			alSourceUnqueueBuffers(m_source, count, buffers);

			const struct AudioResourceInfo *info = m_owner->GetResourceInfo();

			if (info) {
				for (int i = 0; i < count; i++) {
					if (m_queuedBufferIndices[i] == info->bufferCount - 1) {
						m_isLastBufferQueued = false;

						if (!IsLooping() && IsPlaying()) {
							// We reached the end of a non-looping source, stop playback.
							Pause();
						}
					}
				}
			}

			// Move the remaining queued buffers to the start of the array.
			for (int i = count; i < 2; i++) {
				m_queuedBufferIndices[i - count] = m_queuedBufferIndices[i];
			}
			// Clear the end of the array.
			for (int i = 2 - count; i < 2; i++) {
				m_queuedBufferIndices[i] = -1;
			}
		}

		assert(alGetError() == AL_NO_ERROR);
	}

	return (unsigned int)count;
}
