#ifndef SOUND_AUDIORESOURCE_H
#define SOUND_AUDIORESOURCE_H

#include <cstdint>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <AL/al.h>

#include "AudioDecoder.h"

struct MemoryMappedFile
{
	const char *data;
	uint64_t size;

	// Private
	HANDLE fileHandle;
	HANDLE fileMapping;
};

struct AudioResourceInfo
{
	// Filled in by AudioDecoder.init
	ALenum format;
	uint64_t totalSamples;
	uint64_t samplesPerBuffer; // Typically totalSamples if not streaming SOUNDENGINE_STREAMING_BUFFER_SIZE * sampleRate * channels if streaming.
	unsigned int channels;
	unsigned int sampleRate;
	unsigned int bitDepth;

	// These fields are derived from the above fields.
	float duration;
	unsigned int bufferCount;
};

struct AudioResource
{
	enum AudioDecoders decoder;
	struct MemoryMappedFile *file;

	struct AudioResourceInfo info;
	void *context;
};

struct AudioResource *CreateAudioResource(const std::string &file);
void DestroyAudioResource(struct AudioResource *resource);

#endif
