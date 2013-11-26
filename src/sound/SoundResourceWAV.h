#ifndef SOUND_SOUNDRESOURCEWAV_H
#define SOUND_SOUNDRESOURCEWAV_H

#include <string>
#include <fstream>

#include <AL/al.h>

#include "SoundResource.h"
#include "SoundEngineImpl.h"

class SoundResourceWAV : public SoundResource
{
public:
	virtual ~SoundResourceWAV();

	ALuint GetBuffer();
	void FillBuffer(ALuint buffer, size_t sampleOffset, size_t sampleCount);

private:
	// Limit creating SoundResources to the SoundEngine.
	friend class SoundEngineImpl;

	static SoundResourceWAV *Load(SoundEngineImpl *engine, const std::string &name);

	// Constructor for non-streaming.
	SoundResourceWAV(SoundEngineImpl *engine, const std::string &name, const char *data, size_t size, const SoundResourceInfo &info);
	// Constructor for streaming.
	SoundResourceWAV(const std::string &name, size_t dataOffset, const SoundResourceInfo &info);

private:
	// Non-streaming specific members.
	SoundEngineImpl *m_engine;
	ALuint m_buffer;

	// Streaming specific members.
	std::fstream m_file;
	size_t m_dataOffset;
};

#endif
