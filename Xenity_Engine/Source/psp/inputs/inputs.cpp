// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(__PSP__)

#include <pspctrl.h>
#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"

SceCtrlData ctrl;

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
	buttonMap[PSP_CTRL_CROSS] = &inputs[(int)KeyCode::CROSS];
	buttonMap[PSP_CTRL_CIRCLE] = &inputs[(int)KeyCode::CIRCLE];
	buttonMap[PSP_CTRL_SQUARE] = &inputs[(int)KeyCode::SQUARE];
	buttonMap[PSP_CTRL_TRIANGLE] = &inputs[(int)KeyCode::TRIANGLE];

	buttonMap[PSP_CTRL_START] = &inputs[(int)KeyCode::START];
	buttonMap[PSP_CTRL_SELECT] = &inputs[(int)KeyCode::SELECT];
	buttonMap[PSP_CTRL_LTRIGGER] = &inputs[(int)KeyCode::LTRIGGER1];
	buttonMap[PSP_CTRL_RTRIGGER] = &inputs[(int)KeyCode::RTRIGGER1];
	buttonMap[PSP_CTRL_RIGHT] = &inputs[(int)KeyCode::DPAD_RIGHT];
	buttonMap[PSP_CTRL_LEFT] = &inputs[(int)KeyCode::DPAD_LEFT];
	buttonMap[PSP_CTRL_DOWN] = &inputs[(int)KeyCode::DPAD_DOWN];
	buttonMap[PSP_CTRL_UP] = &inputs[(int)KeyCode::DPAD_UP];
}

void CrossInputsInit()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

InputPad CrossGetInputPad(const int controllerId)
{
	InputPad pad = InputPad();
	sceCtrlReadBufferPositive(&ctrl, 1);
	pad.buttons = ctrl.Buttons;

	pad.lx = ((ctrl.Lx - 128) / 256.0f) * 2;
	pad.ly = ((ctrl.Ly - 128) / 256.0f) * 2;

	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	// Should be empty
	std::vector<TouchRaw> touchesRaw;
	return touchesRaw;
}

#endif