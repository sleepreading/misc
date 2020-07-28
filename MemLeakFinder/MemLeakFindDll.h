// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MEMLEAKDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MEMLEAKDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#ifdef MEMLEAKDLL_EXPORTS
#define MEMLEAKDLL_API __declspec(dllexport)
#else
#define MEMLEAKDLL_API __declspec(dllimport)
#endif

MEMLEAKDLL_API void* TraceAlloc(size_t nSize);
MEMLEAKDLL_API void TraceDealloc(void* poMem);
