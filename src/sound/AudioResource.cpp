#include <cassert>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>

#include "AudioResource.h"

// Must be included last!
#include "debug.h"

static bool forceSineWave = false;

struct AudioResource *CreateAudioResource(const std::string &file)
{
	if (forceSineWave) {
		struct AudioResource *resource = new AudioResource;
		resource->decoder = &audioDecoderSineWave;
		resource->file = NULL;

		assert(resource->decoder->init(resource));

		assert(resource->info.totalSamples > 0);
		assert(resource->info.samplesPerBuffer > 0);
		assert(resource->info.channels > 0);
		assert(resource->info.sampleRate > 0);
		assert(resource->info.bitDepth > 0);

		resource->info.duration = ((float)resource->info.totalSamples / resource->info.channels) / resource->info.sampleRate;
		resource->info.bufferCount = (unsigned int)(resource->info.totalSamples / resource->info.samplesPerBuffer);

		if (resource->info.totalSamples % resource->info.samplesPerBuffer > 0) {
			resource->info.bufferCount++;
		}

		printf("duration=%.2f, bufferCount=%u\n", resource->info.duration, resource->info.bufferCount);

		return resource;
	}

	int fd = _open(file.c_str(), O_RDONLY, 0);

	if (fd < 0) {
		printf("Failed to open resource '%s'\n", file.c_str());
		return NULL;
	}
	
	struct stat fileInfo;

	if (fstat(fd, &fileInfo) < 0) {
		printf("Failed to get size of resource '%s'\n", file.c_str());

		_close(fd);

		return NULL;
	}

	void *mappedFile = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	_close(fd);

	if (mappedFile == MAP_FAILED) {
		printf("Failed to map resource '%s'\n", file.c_str());

		return NULL;
	}

	struct AudioResource *resource = new AudioResource;
	resource->decoder = NULL;
	resource->fileSize = fileInfo.st_size;
	resource->file = mappedFile;

	for (int i = 0; i < AudioDecoderCount; i++) {
		if (audioDecoders[i].init(resource)) {
			resource->decoder = &audioDecoders[i];
			break;
		}
	}

	if (!resource->decoder) {
		printf("Failed to decode resource '%s'\n", file.c_str());

		munmap(resource->file, (size_t)resource->fileSize);
		delete resource;
		
		return NULL;
	}

	assert(resource->info.totalSamples > 0);
	assert(resource->info.samplesPerBuffer > 0);
	assert(resource->info.channels > 0);
	assert(resource->info.sampleRate > 0);
	assert(resource->info.bitDepth > 0);

	resource->info.duration = ((float)resource->info.totalSamples / resource->info.channels) / resource->info.sampleRate;
	resource->info.bufferCount = (unsigned int)(resource->info.totalSamples / resource->info.samplesPerBuffer);

	if (resource->info.totalSamples % resource->info.samplesPerBuffer > 0) {
		resource->info.bufferCount++;
	}

	//printf("duration=%.2f, bufferCount=%i\n", resource->info.duration, resource->info.bufferCount);

	return resource;
}

void DestroyAudioResource(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	resource->decoder->release(resource);
	munmap(resource->file, (size_t)resource->fileSize);

	delete resource;
}
