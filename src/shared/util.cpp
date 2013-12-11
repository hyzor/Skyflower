#include <cstdint>

#include "platform.h"

#ifdef PLATFORM_WINDOWS
#include <psapi.h>
#endif

DLL_API uint64_t GetMemoryUsage()
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

DLL_API double GetTime()
{
#ifdef PLATFORM_WINDOWS
	static LARGE_INTEGER frequency;

	if (!frequency.QuadPart)
		QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (double)counter.QuadPart / frequency.QuadPart;
#else
#error Unsupported OS
#endif
}
