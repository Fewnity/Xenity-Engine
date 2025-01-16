// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/reflection/enum_utils.h>

// Do not remove the "P_" because without that, the PSP compiler will not allow the enum
ENUM(Platform,
	P_Windows,
	P_Linux,
	P_PSP,
	P_PsVita,
	//P_PS2,
	P_PS3,
	//P_PS4, 
	P_COUNT);

// Do not remove the "AP_" because without that, the PSP compiler will not allow the enum
ENUM(AssetPlatform,
	AP_Standalone,
	AP_PSP,
	AP_PsVita,
	//AP_PS2,
	AP_PS3,
	//AP_PS4,
	AP_COUNT);

static const char* s_assetPlatformNames[static_cast<int>(AssetPlatform::AP_COUNT)] = {
	"Standalone",
	"PSP",
	"PSVITA",
	//"PS2",
	"PS3"
	//"PS4"
};