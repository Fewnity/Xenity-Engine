// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/reflection/enum_utils.h>

// Do not remove the "P_" because without that, the PSP compiler will not allow the enum
ENUM(Platform,
	P_Windows,
	P_PSP,
	P_PsVita,
	P_PS2,
	P_PS3,
	P_PS4);