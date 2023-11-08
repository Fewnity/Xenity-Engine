#if defined(_WIN32) || defined(_WIN64)

#include "../engine/cpu.h"
#include <windows.h>

void SetMaxCpuSpeed()
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

#endif