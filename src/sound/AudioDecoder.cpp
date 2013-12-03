#include "AudioDecoder.h"
#include "AudioDecoderWAV.h"

const struct AudioDecoder audioDecoders[AudioDecoderCount] = {
	// AudioDecoderWAV
	{ AudioDecoderWAVInit, AudioDecoderWAVRelease, AudioDecoderWAVFillBuffer }
};
