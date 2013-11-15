#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
	#ifdef COMPILING_DLL
		#define DLL_API __declspec(dllexport)
	#else
		#define DLL_API __declspec(dllimport)
	#endif
#else
	#define DLL_API
#endif

#endif
