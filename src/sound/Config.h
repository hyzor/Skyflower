#ifndef SOUND_CONFIG_H
#define SOUND_CONFIG_H

#define SOUNDENGINE_BUFFER_POOL_SIZE       128
// Sound files larger than this will be streamed.
#define SOUNDENGINE_STREAMING_SIZE_LIMIT   (1 * 1024 * 1024)
// Maximum number of buffers that can be queued at once for a streaming source.
#define SOUNDENGINE_STREAMING_BUFFER_COUNT 3
// How many samples, in seconds, each buffer will hold for streaming resources.
#define SOUNDENGINE_STREAMING_BUFFER_SIZE  0.25

#endif
