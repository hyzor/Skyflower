#ifndef SOUND_AUDIODECODERSINEWAVE_H
#define SOUND_AUDIODECODERSINEWAVE_H

#include "AudioDecoder.h"

bool AudioDecoderSineWaveInit(struct AudioResource *resource);
void AudioDecoderSineWaveRelease(struct AudioResource *resource);

void AudioDecoderSineWaveFillBuffer(const struct AudioResource *resource, uint64_t sampleOffset, uint64_t sampleCount, ALuint buffer);

#endif
