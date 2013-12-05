#ifndef SOUND_AUDIODECODERWAV_H
#define SOUND_AUDIODECODERWAV_H

#include <cstdint>

#include <AL/al.h>

bool AudioDecoderWAVInit(struct AudioResource *resource);
void AudioDecoderWAVRelease(struct AudioResource *resource);

void AudioDecoderWAVFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer);

#endif
