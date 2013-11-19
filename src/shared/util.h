#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include <cstdint>

#include "shared/platform.h"

#ifdef PLATFORM_WINDOWS
#include <psapi.h>
#endif

// Returns the application's memory usage in bytes.
uint64_t GetMemoryUsage()
{
#ifdef PLATFORM_WINDOWS
	PROCESS_MEMORY_COUNTERS info;

	if (GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)) == 0)
		return 0;

	return (uint64_t)info.WorkingSetSize;
#else
#error Unsupported OS
#endif
}

// Returns the current time in seconds.
float GetTime()
{
#ifdef PLATFORM_WINDOWS
	static LARGE_INTEGER frequency;

	if (!frequency.QuadPart)
		QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (float)counter.QuadPart / frequency.QuadPart;
#else
#error Unsupported OS
#endif
}

#endif
