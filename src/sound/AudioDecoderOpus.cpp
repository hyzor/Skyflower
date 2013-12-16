#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>

#include <opusfile.h>

#include "AudioDecoderOpus.h"
#include "AudioResource.h"
#include "Config.h"

bool AudioDecoderOpusInit(struct AudioResource *resource)
{
	OggOpusFile *opus = op_open_memory((const unsigned char *)resource->file->data, (size_t)resource->file->size, NULL);

	if (!opus) {
		return false;
	}

	int64_t totalSamples = op_pcm_total(opus, -1);
	int channels = op_channel_count(opus, -1);

	if (op_seekable(opus) == 0 || totalSamples < 0 || channels > 2) {
		op_free(opus);
		return false;
	}

	// Opus is fixed at 16-bit and 48kHz sample rate.
	unsigned int bitDepth = 16;
	unsigned int sampleRate = 48000;
	//bool shouldStream = (resource->file->size > SOUNDENGINE_STREAM_THRESHOLD_SIZE? true : false);
	bool shouldStream = false;

	resource->info.format = (channels == 1? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	resource->info.totalSamples = totalSamples;
	resource->info.samplesPerBuffer = (shouldStream? std::min((int64_t)(SOUNDENGINE_STREAM_BUFFER_SIZE * sampleRate * channels), totalSamples) : totalSamples);
	resource->info.channels = channels;
	resource->info.sampleRate = sampleRate;
	resource->info.bitDepth = bitDepth;
	resource->context = (void *)opus;

	//printf("AudioDecoderOpus, shouldStream=%d, bufferCount=%d, channels=%d, sampleRate=%d, bitDepth=%d\n", shouldStream, (int)ceil(resource->info.totalSamples / (float)resource->info.samplesPerBuffer), channels, sampleRate, bitDepth);

	return true;
}

void AudioDecoderOpusRelease(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	OggOpusFile *opus = (OggOpusFile *)resource->context;
	op_free(opus);
}

void AudioDecoderOpusFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer)
{
	assert(resource);
	assert(sampleOffset + sampleCount <= resource->info.totalSamples);
	assert(alIsBuffer(buffer));

	OggOpusFile *opus = (OggOpusFile *)resource->context;
	int64_t position = op_pcm_tell(opus);

	assert(position >= 0);

	if (position != sampleOffset) {
		assert(op_pcm_seek(opus, sampleOffset) == 0);
	}

	// FIXME: Don't alloc this every time.
	int16_t *data = new int16_t[(size_t)sampleCount];
	int samplesWritten = 0;
	
	while (samplesWritten < sampleCount) {
		int result = op_read(opus, data + samplesWritten, (int)sampleCount - samplesWritten, NULL);

		assert(result > 0);

		// op_read returns the number of samples written per channel.
		samplesWritten += result * resource->info.channels;
	}

	alBufferData(buffer, resource->info.format, (const void *)data, (ALsizei)(sampleCount * 2), resource->info.sampleRate);
	delete[] data;

	assert(alGetError() == AL_NO_ERROR);
}
