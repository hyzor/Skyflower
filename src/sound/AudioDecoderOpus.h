#ifndef SOUND_AUDIODECODEROPUS_H
#define SOUND_AUDIODECODEROPUS_H

#include "AudioDecoder.h"

bool AudioDecoderOpusInit(struct AudioResource *resource);
void AudioDecoderOpusRelease(struct AudioResource *resource);

void AudioDecoderOpusFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer);

#endif
