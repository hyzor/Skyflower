#define _USE_MATH_DEFINES
#include <cmath>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>

#include <AL/alext.h>

#include "AudioDecoderSineWave.h"
#include "AudioResource.h"
#include "Config.h"

// Must be included last!
#include "debug.h"

struct SineWaveDecoderContext
{
	// In number of samples.
	size_t bufferSize;
	float *buffer;
};

bool AudioDecoderSineWaveInit(struct AudioResource *resource)
{
	double duration = 10.0;
	unsigned int sampleRate = 48000;
	uint64_t totalSamples = (uint64_t)(sampleRate * duration);
	uint64_t samplesPerBuffer = std::min((uint64_t)sampleRate, totalSamples);
	//uint64_t samplesPerBuffer = totalSamples;

	struct SineWaveDecoderContext *context = new struct SineWaveDecoderContext;
	context->bufferSize = (size_t)samplesPerBuffer;
	context->buffer = new float[context->bufferSize];

	double amplitude = 0.5;
	double frequency = 1000.0;

	for(uint64_t i = 0; i < context->bufferSize; i++) {
		context->buffer[i] = (float)(amplitude * sin(2.0 * M_PI * frequency / sampleRate * i));
	}

	resource->info.format = AL_FORMAT_MONO_FLOAT32;
	resource->info.totalSamples = totalSamples;
	resource->info.samplesPerBuffer = samplesPerBuffer;
	resource->info.channels = 1;
	resource->info.sampleRate = sampleRate;
	resource->info.bitDepth = 32;
	resource->context = (void *)context;

	return true;
}

void AudioDecoderSineWaveRelease(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	struct SineWaveDecoderContext *context = (struct SineWaveDecoderContext *)resource->context;

	delete[] context->buffer;
	delete context;
}

void AudioDecoderSineWaveFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer)
{
	assert(resource);
	assert(sampleOffset + sampleCount <= resource->info.totalSamples);
	assert(alIsBuffer(buffer));

	struct SineWaveDecoderContext *context = (struct SineWaveDecoderContext *)resource->context;

	assert(sampleCount == context->bufferSize);

	alBufferData(buffer, resource->info.format, (const void *)context->buffer, (ALsizei)(context->bufferSize * sizeof(*context->buffer)), resource->info.sampleRate);

	assert(alGetError() == AL_NO_ERROR);
}
