#ifndef SOUND_RESOURCECACHE_H
#define SOUND_RESOURCECACHE_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include <AL/al.h>

#include "AudioResource.h"
#include "Config.h"
#include "TaskQueue.h"

enum BufferStatus
{
	BufferStatusLoadingHeaders = 0,
	BufferStatusPendingLoad,
	BufferStatusLoaded,
	BufferStatusIndexOutOfBounds,
	BufferStatusOutOfMemory,
	// Can mean either the file was not found or it's in an unsupported format.
	BufferStatusInvalidFile
};

struct ResourceCacheEntry
{
	uint32_t resourceHash;
	unsigned int bufferIndex;
	uint64_t lastRequestedFrame;
	ALuint buffer;
};

class ResourceCache
{
public:
	ResourceCache(const std::string &resourceDir);
	virtual ~ResourceCache();

	void Update();

	uint32_t PrefetchResource(const std::string &name);
	// resourceHash must be a value returned by PrefetchResource.
	enum BufferStatus RequestBuffer(uint32_t resourceHash, unsigned int bufferIndex, ALuint *buffer_out);

	const struct AudioResourceInfo *GetResourceInfo(uint32_t resourceHash);
	bool IsResourceStreaming(uint32_t resourceHash);
	unsigned int ConvertTimeToBufferIndex(uint32_t resourceHash, float time, uint64_t *sampleOffset_out);

private:
	std::string m_resourceDir;
	TaskQueue *m_taskQueue;
	uint64_t m_currentFrame;

	ALuint m_buffers[SOUNDENGINE_BUFFER_POOL_SIZE];
	struct ResourceCacheEntry m_cacheEntries[SOUNDENGINE_BUFFER_POOL_SIZE];
	bool m_cacheEntryLoading[SOUNDENGINE_BUFFER_POOL_SIZE];

	std::unordered_map<uint32_t, AudioResource *> m_resources;
	std::unordered_map<uint32_t, bool> m_resourcesLoading;
};

#endif
