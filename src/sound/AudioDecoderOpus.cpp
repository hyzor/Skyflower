#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>

#include <AL/alext.h>
#include <opusfile.h>

#include "AudioDecoderOpus.h"
#include "AudioResource.h"
#include "Config.h"
#include "TaskQueueWin32.h"

// Must be included last!
#include "debug.h"

struct OpusDecoderContext
{
	OggOpusFile *opus;
	Mutex *mutex;
	// In number of samples.
	size_t bufferSize;
	float *buffer;
};

bool AudioDecoderOpusInit(struct AudioResource *resource)
{
	OggOpusFile *opus = op_open_memory((const unsigned char *)resource->file->data, (size_t)resource->file->size, NULL);

	if (!opus) {
		return false;
	}

	int channels = op_channel_count(opus, -1);
	int64_t totalSamples = op_pcm_total(opus, -1) * channels;

	if (op_seekable(opus) == 0 || channels > 2 || totalSamples < 0) {
		op_free(opus);
		return false;
	}

	// Opus is fixed at 48kHz sample rate and outputs 32-bit floating point values.
	unsigned int sampleRate = 48000;
	unsigned int bitDepth = 32;
	bool shouldStream = ((totalSamples * (bitDepth / 8)) > SOUNDENGINE_STREAM_THRESHOLD_SIZE? true : false);
	uint64_t samplesPerBuffer = (shouldStream? std::min((int64_t)(SOUNDENGINE_STREAM_BUFFER_SIZE * sampleRate * channels), totalSamples) : totalSamples);

	struct OpusDecoderContext *context = new struct OpusDecoderContext;
	context->opus = opus;
	context->mutex = new MutexWin32();
	context->bufferSize = 0;
	context->buffer = NULL;

	if (shouldStream) {
		context->bufferSize = (size_t)samplesPerBuffer;
		context->buffer = new float[context->bufferSize];
	}

	resource->info.format = (channels == 1? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32);
	resource->info.totalSamples = totalSamples;
	resource->info.samplesPerBuffer = samplesPerBuffer;
	resource->info.channels = channels;
	resource->info.sampleRate = sampleRate;
	resource->info.bitDepth = bitDepth;
	resource->context = (void *)context;

	//printf("AudioDecoderOpus, shouldStream=%d, bufferCount=%d, channels=%d, sampleRate=%d, bitDepth=%d\n", shouldStream, (int)ceil(resource->info.totalSamples / (float)resource->info.samplesPerBuffer), channels, sampleRate, bitDepth);

	return true;
}

void AudioDecoderOpusRelease(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	struct OpusDecoderContext *context = (struct OpusDecoderContext *)resource->context;

	delete[] context->buffer;
	delete context->mutex;
	op_free(context->opus);

	delete context;
}

void AudioDecoderOpusFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer)
{
	assert(resource);
	assert(sampleOffset + sampleCount <= resource->info.totalSamples);
	assert(alIsBuffer(buffer));

	struct OpusDecoderContext *context = (struct OpusDecoderContext *)resource->context;
	context->mutex->Lock();

	int64_t position = op_pcm_tell(context->opus) * resource->info.channels;

	assert(position >= 0);

	if (position != sampleOffset) {
		assert(op_pcm_seek(context->opus, sampleOffset) == 0);
	}

	uint64_t samplesRead = 0;
	float *data;

	if (context->buffer) {
		assert(context->bufferSize >= sampleCount);

		data = context->buffer;
	}
	else {
		data = new float[(size_t)sampleCount];
	}

	while (samplesRead < sampleCount) {
		int result = op_read_float(context->opus, data + samplesRead, (int)(sampleCount - samplesRead), NULL);

		assert(result > 0);

		// op_read returns the number of samples written per channel.
		samplesRead += (uint64_t)result * resource->info.channels;
	}

	assert(samplesRead <= sampleCount);

	alBufferData(buffer, resource->info.format, (const void *)data, (ALsizei)(samplesRead * sizeof(*data)), resource->info.sampleRate);

	if (!context->buffer) {
		delete[] data;
	}

	context->mutex->Unlock();

	assert(alGetError() == AL_NO_ERROR);
}
