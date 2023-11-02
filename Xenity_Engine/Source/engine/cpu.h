#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

/**
* Set Cpu speed to the max (PSP and PsVita)
*/
API void SetMaxCpuSpeed();