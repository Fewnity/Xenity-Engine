// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__vita__)

#include "../engine/cpu.h"
#include <psp2/power.h>

void SetMaxCpuSpeed()
{
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
}
#endif