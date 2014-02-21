#ifndef SOUND_AUDIORESOURCE_H
#define SOUND_AUDIORESOURCE_H

#include <cstdint>
#include <string>

#ifdef _WIN32
#include "win32/mman.h"
#else
#include <sys/mman.h>
#endif

#include <AL/al.h>

#include "AudioDecoder.h"

struct AudioResourceInfo
{
	// Filled in by AudioDecoder.init
	uint64_t totalSamples;
	uint64_t samplesPerBuffer; // Typically totalSamples if not streaming SOUNDENGINE_STREAMING_BUFFER_SIZE * sampleRate * channels if streaming.
	unsigned int channels;
	unsigned int sampleRate;
	unsigned int bitDepth;
	ALenum format;

	// These fields are derived from the above fields.
	float duration;
	unsigned int bufferCount;
};

struct AudioResource
{
	const struct AudioDecoder *decoder;
	struct AudioResourceInfo info;

	uint64_t fileSize;
	void *file;

	void *context;
};

struct AudioResource *CreateAudioResource(const std::string &file);
void DestroyAudioResource(struct AudioResource *resource);

#endif
