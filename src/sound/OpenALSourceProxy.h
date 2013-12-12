#ifndef SOUND_OPENALSOURCEPROXY_H
#define SOUND_OPENALSOURCEPROXY_H

#include <cstdint>

#include <AL/al.h>

class SoundSourceImpl;

class OpenALSourceProxy
{
public:
	OpenALSourceProxy(const SoundSourceImpl *owner);
	virtual ~OpenALSourceProxy();

	void LendSource(ALuint source);
	ALuint RevokeSource();
	bool HasSource() const;

	void ClearBuffers();

	void Update(float deltaTime);

	void Play();
	void Pause();
	void Seek(float time, uint64_t sample);

	void SetBuffer(ALuint buffer);
	void SetLooping(bool looping);
	void SetRelativeToListener(bool relative);
	void SetVolume(float volume);
	void SetPitch(float pitch);
	void SetPosition(const float position[3]);
	void SetVelocity(const float velocity[3]);

	ALuint GetBuffer() const;
	unsigned int GetNumQueuedBuffers() const;
	bool IsPlaying() const;
	bool IsLooping() const;
	bool IsRelativeToListener() const;
	float GetVolume() const;
	float GetPitch() const;
	void GetPosition(float output[3]) const;
	void GetVelocity(float output[3]) const;
	// The playback time is only updated when we don't have a source, therefore it should not
	// be called when we have a source.
	float GetPlaybackTime() const;

	void QueueBuffer(ALuint buffer, unsigned int bufferIndex);
	unsigned int UnqueueProcessedBuffers();

private:
	const SoundSourceImpl *m_owner;
	ALuint m_source;

	ALuint m_buffer;
	bool m_isPlaying;
	bool m_isLooping;
	bool m_isRelativeToListener;
	float m_volume;
	float m_pitch;
	float m_position[3];
	float m_velocity[3];

	float m_time;
	int m_queuedBufferIndices[2];
};

#endif
