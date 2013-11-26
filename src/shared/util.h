#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include <cstdint>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0])) 

// Returns the application's memory usage in bytes.
uint64_t GetMemoryUsage();
// Returns the current time in seconds.
float GetTime();

#endif