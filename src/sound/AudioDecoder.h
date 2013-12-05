#ifndef SOUND_AUDIODECODER_H
#define SOUND_AUDIODECODER_H

#include <cstdint>

#include <AL/al.h>

#include "AudioResource.h"

enum AudioDecoders
{
	AudioDecoderWAV = 0,
	AudioDecoderCount
};

struct AudioDecoder
{
	bool (*init)(struct AudioResource *resource);
	void (*release)(struct AudioResource *resource);

	void (*fillBuffer)(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer);
};

extern const struct AudioDecoder audioDecoders[AudioDecoderCount];

#endif
