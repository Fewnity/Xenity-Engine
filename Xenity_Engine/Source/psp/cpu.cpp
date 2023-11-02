#if defined(__PSP__)

#include "../engine/cpu.h"
#include <psppower.h>

void SetMaxCpuSpeed()
{
	scePowerSetClockFrequency(333, 333, 166);
}
#endif