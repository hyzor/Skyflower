#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include <cstdint>

#include "shared/platform.h"

#ifdef PLATFORM_WINDOWS
#include <psapi.h>
#endif

#ifdef PLATFORM_WINDOWS
// Returns the application's memory usage in bytes.
uint64_t GetMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS info;

	if (GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)) == 0)
		return 0;

	return (uint64_t)info.WorkingSetSize;
}
#endif

#endif
