#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>

#include "AudioDecoderWAV.h"
#include "AudioResource.h"
#include "Config.h"

// Must be included last!
#include "debug.h"

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((uint32_t)(((d) << 24) | ((c) << 16) | ((b) << 8) | (a)))
#endif

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#endif

struct WAVDecoderContext
{
	uint64_t dataOffset;
};

struct ChunkHeader
{
	uint32_t fourcc;
	uint32_t size;
};

struct HeaderChunk
{
	ChunkHeader base;
	uint32_t format;
};

struct FormatChunk
{
	ChunkHeader base;
	uint16_t format;
	uint16_t channels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitDepth;
};

bool AudioDecoderWAVInit(struct AudioResource *resource)
{
	const char *buffer = resource->file->data;
	const struct HeaderChunk *header = (const struct HeaderChunk *)buffer;

	if (header->base.fourcc != FOURCC('R', 'I', 'F', 'F') || header->format != FOURCC('W', 'A', 'V', 'E')) {
		return false;
	}

	buffer += sizeof(struct HeaderChunk);
	const struct FormatChunk *format = (const struct FormatChunk *)buffer;
	
	if (format->base.fourcc != FOURCC('f', 'm', 't', ' ') || format->base.size != 16 || format->format != WAVE_FORMAT_PCM) {
		return false;
	}
	if (format->bitDepth != 8 && format->bitDepth != 16) {
		return false;
	}

	unsigned int channels = format->channels;
	unsigned int sampleRate = format->sampleRate;
	unsigned int bitDepth = format->bitDepth;
	ALenum bufferFormat;

	switch (format->channels) {
	case 1:
		bufferFormat = (format->bitDepth == 8? AL_FORMAT_MONO8 : AL_FORMAT_MONO16);
		break;
	case 2:
		bufferFormat = (format->bitDepth == 8? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);
		break;
	default:
		return false;
	}

	buffer += sizeof(struct FormatChunk);
	const struct ChunkHeader *dataHeader = (const struct ChunkHeader *)buffer;
	uint64_t chunkSize = 0;

	do {
		if (buffer + chunkSize > resource->file->data + resource->file->size) {
			return false;
		}

		buffer += chunkSize;
		dataHeader = (const struct ChunkHeader *)buffer;
		chunkSize = dataHeader->size;
	} while (dataHeader->fourcc != FOURCC('d', 'a', 't', 'a'));

	uint64_t totalSamples = dataHeader->size / (bitDepth / 8);
	bool shouldStream = (dataHeader->size > SOUNDENGINE_STREAM_THRESHOLD_SIZE? true : false);

	struct WAVDecoderContext *context = new struct WAVDecoderContext;
	context->dataOffset = (buffer - resource->file->data) + sizeof(struct ChunkHeader);

	resource->info.format = bufferFormat;
	resource->info.totalSamples = totalSamples;
	resource->info.samplesPerBuffer = (shouldStream? std::min((uint64_t)(SOUNDENGINE_STREAM_BUFFER_SIZE * sampleRate * channels), totalSamples) : totalSamples);
	resource->info.channels = channels;
	resource->info.sampleRate = sampleRate;
	resource->info.bitDepth = bitDepth;
	resource->context = (void *)context;

	//printf("AudioDecoderWAV, shouldStream=%d, bufferCount=%d, channels=%d, sampleRate=%d, bitDepth=%d\n", shouldStream, (int)ceil(resource->info.totalSamples / (float)resource->info.samplesPerBuffer), channels, sampleRate, bitDepth);

	return true;
}

void AudioDecoderWAVRelease(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	struct WAVDecoderContext *context = (struct WAVDecoderContext *)resource->context;

	delete context;
}

void AudioDecoderWAVFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer)
{
	assert(resource);
	assert(sampleOffset + sampleCount <= resource->info.totalSamples);
	assert(alIsBuffer(buffer));

	struct WAVDecoderContext *context = (struct WAVDecoderContext *)resource->context;

	uint64_t byteOffset = context->dataOffset + sampleOffset * (resource->info.bitDepth / 8);
	uint64_t size = sampleCount * (resource->info.bitDepth / 8);

	alBufferData(buffer, resource->info.format, (const void *)(resource->file->data + byteOffset), (ALsizei)size, resource->info.sampleRate);

	assert(alGetError() == AL_NO_ERROR);
}
