// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(_EE)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"

#define JOYSTICK_DEAD_ZONE 0.25f

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
}

void CrossInputsInit()
{
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();
	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	// Should be empty
	std::vector<TouchRaw> touchesRaw;
	return touchesRaw;
}

#endif