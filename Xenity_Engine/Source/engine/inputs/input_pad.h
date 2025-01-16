// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

struct InputPad
{
	std::map<int, bool> pressedButtons;
	float lx = 0;
	float ly = 0;
	float rx = 0;
	float ry = 0;
	int buttons = 0;
};