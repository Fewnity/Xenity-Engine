// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PSP__)

#include "../engine/cpu.h"
#include <psppower.h>

void SetMaxCpuSpeed()
{
	scePowerSetClockFrequency(333, 333, 166);
}
#endif