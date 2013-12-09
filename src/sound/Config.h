#ifndef SOUND_CONFIG_H
#define SOUND_CONFIG_H

// How many times per second the sound sources should be culled.
#define SOUNDENGINE_CULLING_FREQUENCY      1
#define SOUNDENGINE_CULLING_DISTANCE       100.0

#define SOUNDENGINE_SOURCE_POOL_SIZE       128
// Twice as many since streaming sources will use double buffering.
#define SOUNDENGINE_BUFFER_POOL_SIZE       (SOUNDENGINE_SOURCE_POOL_SIZE * 2)

// Sound files larger than this will be streamed.
#define SOUNDENGINE_STREAM_THRESHOLD_SIZE  (1 * 1024 * 1024)
// How many samples, in seconds, each buffer will hold for streaming resources.
#define SOUNDENGINE_STREAM_BUFFER_SIZE     0.25

#endif
