#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include <cstdint>

// Returns the application's memory usage in bytes.
uint64_t GetMemoryUsage();
// Returns the current time in seconds.
float GetTime();

#endif
