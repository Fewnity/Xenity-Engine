// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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

	keyMap[PAD_CTRL_CROSS] = &inputs[(int)KeyCode::CROSS];
	keyMap[PAD_CTRL_CIRCLE] = &inputs[(int)KeyCode::CIRCLE];
	keyMap[PAD_CTRL_SQUARE] = &inputs[(int)KeyCode::SQUARE];
	keyMap[PAD_CTRL_TRIANGLE] = &inputs[(int)KeyCode::TRIANGLE];

	keyMap[PAD_CTRL_START << 8] = &inputs[(int)KeyCode::START];
	keyMap[PAD_CTRL_SELECT << 8] = &inputs[(int)KeyCode::SELECT];

	keyMap[PAD_CTRL_L1] = &inputs[(int)KeyCode::LTRIGGER1];
	keyMap[PAD_CTRL_R1] = &inputs[(int)KeyCode::RTRIGGER1];

	keyMap[PAD_CTRL_L3 << 8] = &inputs[(int)KeyCode::L_JOYSTICK_CLICK];
	keyMap[PAD_CTRL_R3 << 8] = &inputs[(int)KeyCode::R_JOYSTICK_CLICK];

	keyMap[PAD_CTRL_RIGHT << 8] = &inputs[(int)KeyCode::DPAD_RIGHT];
	keyMap[PAD_CTRL_LEFT << 8] = &inputs[(int)KeyCode::DPAD_LEFT];
	keyMap[PAD_CTRL_DOWN << 8] = &inputs[(int)KeyCode::DPAD_DOWN];
	keyMap[PAD_CTRL_UP << 8] = &inputs[(int)KeyCode::DPAD_UP];
}

void CrossInputsInit()
{
	ioPadInit(7);
	// ioPadSetPressMode(0, PAD_PRESS_MODE_ON);
	// ioPadSetPortSetting(0, PAD_SETTINGS_PRESS_ON);
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();

	padInfo padinfo;
	ioPadGetInfo(&padinfo);

	const uint32_t controllerIndex = 0;
	if(padinfo.status[controllerIndex]) 
	{
		padData paddata = padData();
		ioPadGetData(controllerIndex, &paddata);

		// Read buttons
		const u32 btn = ((paddata.button[2] << 8) | (paddata.button[3] & 0xff));
		pad.buttons = btn;
		uint32_t CELL_UTIL_ANALOG_RIGHT = 4 << 16;
		uint32_t CELL_UTIL_ANALOG_LEFT = 6 << 16;

		uint32_t roffset = (CELL_UTIL_ANALOG_RIGHT >> 16) & 0x00FF;
		int rjx = paddata.button[roffset];
		int rjy = paddata.button[roffset + 1];

		//  printf("r0 : %d\n", rjx);
		//  printf("r1 : %d\n", rjy);

		uint32_t loffset = (CELL_UTIL_ANALOG_LEFT >> 16) & 0x00FF;
		int ljx = paddata.button[loffset];
		int ljy = paddata.button[loffset + 1];

		//  printf("l0 : %d\n", ljx);
		//  printf("l1 : %d\n", ljy);


		pad.lx = ((ljx - 128) / 256.0f) * 2;
		pad.ly = ((ljy - 128) / 256.0f) * 2;

		// Right joystick
		pad.rx = ((rjx - 128) / 256.0f) * 2;
		pad.ry = ((rjy - 128) / 256.0f) * 2;
		
		// Left joystick
		// pad.lx = (((paddata.ANA_L_H & 0xffff) - 128) / 256.0f) * 2;
		// pad.ly = ((((paddata.ANA_L_V) & 0xffff) - 128) / 256.0f) * 2;

		// // Right joystick
		// pad.rx = (((paddata.ANA_R_H & 0xffff) - 128) / 256.0f) * 2;
		// pad.ry = ((((paddata.ANA_R_V) & 0xffff) - 128) / 256.0f) * 2;

// 		printf("ANA_L_H: 0x%X\n", paddata.ANA_L_H);  // Affiche ANA_L_H en hexadécimal
//     	printf("ANA_L_V: 0x%X\n", paddata.ANA_L_V);  // Affiche ANA_L_V en hexadécimal
// printf("s0 : %d\n", sizeof(padData));
// printf("s1 : %d\n", sizeof(padData2));
    	// Debug::Print("x " + std::to_string(paddata.ANA_L_H));
		// Debug::Print("x2 " + std::to_string(paddata.ANA_L_H& 0xff));
		// Debug::Print("x3 " + std::to_string(paddata.ANA_L_H << 8));

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