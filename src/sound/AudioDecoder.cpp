#include "AudioDecoder.h"
#include "AudioDecoderWAV.h"
#include "AudioDecoderOpus.h"

const struct AudioDecoder audioDecoders[AudioDecoderCount] = {
	{ AudioDecoderWAVInit,  AudioDecoderWAVRelease,  AudioDecoderWAVFillBuffer },
	{ AudioDecoderOpusInit, AudioDecoderOpusRelease, AudioDecoderOpusFillBuffer }
};
