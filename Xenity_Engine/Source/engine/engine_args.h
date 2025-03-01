// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

enum class DevKitRunningMode
{
	FromHDD,
	FromPC
};

/**
* @brief Contains all engine execution parameters
*/
class EngineArgs
{
public:
	bool runningOnDevKit = false;
	DevKitRunningMode devKitRunningMode = DevKitRunningMode::FromHDD;
};