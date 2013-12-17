#include "AudioDecoder.h"
#include "AudioDecoderWAV.h"
#include "AudioDecoderOpus.h"
#include "AudioDecoderSineWave.h"

const struct AudioDecoder audioDecoders[AudioDecoderCount] = {
	{ AudioDecoderWAVInit,  AudioDecoderWAVRelease,  AudioDecoderWAVFillBuffer },
	{ AudioDecoderOpusInit, AudioDecoderOpusRelease, AudioDecoderOpusFillBuffer }
};

const struct AudioDecoder audioDecoderSineWave = { AudioDecoderSineWaveInit, AudioDecoderSineWaveRelease, AudioDecoderSineWaveFillBuffer };
