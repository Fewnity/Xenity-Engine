// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(__PS3__)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"

#include <io/pad.h> 
#include <engine/debug/debug.h>

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
	// Using bitshift on PAD_BUTTON_OFFSET_DIGITAL1 defines because PS3 inputs are weird

	buttonMap[PAD_CTRL_CROSS] = &inputs[(int)KeyCode::CROSS];
	buttonMap[PAD_CTRL_CIRCLE] = &inputs[(int)KeyCode::CIRCLE];
	buttonMap[PAD_CTRL_SQUARE] = &inputs[(int)KeyCode::SQUARE];
	buttonMap[PAD_CTRL_TRIANGLE] = &inputs[(int)KeyCode::TRIANGLE];

	buttonMap[PAD_CTRL_START << 8] = &inputs[(int)KeyCode::START];
	buttonMap[PAD_CTRL_SELECT << 8] = &inputs[(int)KeyCode::SELECT];

	buttonMap[PAD_CTRL_L1] = &inputs[(int)KeyCode::LTRIGGER1];
	buttonMap[PAD_CTRL_R1] = &inputs[(int)KeyCode::RTRIGGER1];

	buttonMap[PAD_CTRL_L3 << 8] = &inputs[(int)KeyCode::L_JOYSTICK_CLICK];
	buttonMap[PAD_CTRL_R3 << 8] = &inputs[(int)KeyCode::R_JOYSTICK_CLICK];

	buttonMap[PAD_CTRL_RIGHT << 8] = &inputs[(int)KeyCode::DPAD_RIGHT];
	buttonMap[PAD_CTRL_LEFT << 8] = &inputs[(int)KeyCode::DPAD_LEFT];
	buttonMap[PAD_CTRL_DOWN << 8] = &inputs[(int)KeyCode::DPAD_DOWN];
	buttonMap[PAD_CTRL_UP << 8] = &inputs[(int)KeyCode::DPAD_UP];
}

void CrossInputsInit()
{
	ioPadInit(7);
	// Enable analog triggers
	for (size_t i = 0; i < MAX_PORT_NUM; i++)
	{
		ioPadSetPressMode(i, PAD_PRESS_MODE_ON);
	}
}

InputPad oldPad[MAX_PORT_NUM] { InputPad() };
padInfo padinfo;

InputPad CrossGetInputPad(const int controllerIndex)
{
	InputPad pad = InputPad();

	if (controllerIndex == 0)
	{
		ioPadGetInfo(&padinfo);
	}

	if (controllerIndex >= padinfo.connected)
	{
		return pad;
	}

	if (padinfo.status[controllerIndex])
	{
		padData paddata = padData();
		ioPadGetData(controllerIndex, &paddata);

		// Check the len to detect if there is new update, if not, the structure is just full of 0
		if (paddata.len != 0)
		{
			// Read buttons
			const u32 btn = ((paddata.button[2] << 8) | (paddata.button[3] & 0xff));
			pad.buttons = btn;

			// Left joystick
			pad.lx = ((paddata.ANA_L_H - 128) / 256.0f) * 2;
			pad.ly = ((paddata.ANA_L_V - 128) / 256.0f) * 2;

			// Right joystick
			pad.rx = ((paddata.ANA_R_H - 128) / 256.0f) * 2;
			pad.ry = ((paddata.ANA_R_V - 128) / 256.0f) * 2;

			// Triggers
			pad.leftTrigger = paddata.PRE_L2 / 255.0f;
			pad.rightTrigger = paddata.PRE_R2 / 255.0f;

			oldPad[controllerIndex] = pad;
		}
		else
		{
			pad = oldPad[controllerIndex];
		}

		ioPadClearBuf(controllerIndex);
	}

	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	// Should be empty
	std::vector<TouchRaw> touchesRaw;
	return touchesRaw;
}

#endif