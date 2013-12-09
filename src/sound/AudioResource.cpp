#include <cassert>

#include "AudioResource.h"

static struct MemoryMappedFile *MemoryMapFile(const char *file)
{
	HANDLE fileHandle = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	LARGE_INTEGER fileSize;
	
	if (GetFileSizeEx(fileHandle, &fileSize) == 0) {
		CloseHandle(fileHandle);
		return NULL;
	}

	HANDLE fileMapping = CreateFileMapping(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);

	if (fileMapping == NULL) {
		CloseHandle(fileHandle);
		return NULL;
	}

	const char *data = (const char *)MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);

	if (data == NULL) {
		CloseHandle(fileMapping);
		CloseHandle(fileHandle);
		return NULL;
	}

	struct MemoryMappedFile *mappedFile = new MemoryMappedFile;
	mappedFile->data = data;
	mappedFile->size = fileSize.QuadPart;
	mappedFile->fileHandle = fileHandle;
	mappedFile->fileMapping = fileMapping;

	return mappedFile;
}

static void FreeMemoryMappedFile(struct MemoryMappedFile *mappedFile)
{
	UnmapViewOfFile(mappedFile->data);
	CloseHandle(mappedFile->fileMapping);
	CloseHandle(mappedFile->fileHandle);

	delete mappedFile;
}

struct AudioResource *CreateAudioResource(const std::string &file)
{
	struct MemoryMappedFile *mappedFile = MemoryMapFile(file.c_str());

	if (!mappedFile) {
		printf("Failed to find resource '%s'\n", file.c_str());
		return NULL;
	}

	struct AudioResource *resource = new AudioResource;
	resource->file = mappedFile;

	bool success = false;

	for (int i = 0; i < AudioDecoderCount; i++) {
		if (audioDecoders[i].init(resource)) {
			success = true;
			resource->decoder = (AudioDecoders)i;
			break;
		}
	}

	if (!success) {
		printf("Failed to decode resource '%s'\n", file.c_str());

		FreeMemoryMappedFile(mappedFile);
		delete resource;
		
		return NULL;
	}

	assert(resource->totalSamples > 0);
	assert(resource->samplesPerBuffer > 0);
	assert(resource->channels > 0);
	assert(resource->sampleRate > 0);
	assert(resource->bitDepth > 0);

	return resource;
}

void DestroyAudioResource(struct AudioResource *resource)
{
	if (!resource) {
		return;
	}

	audioDecoders[resource->decoder].release(resource);

	FreeMemoryMappedFile(resource->file);
	delete resource;
}
