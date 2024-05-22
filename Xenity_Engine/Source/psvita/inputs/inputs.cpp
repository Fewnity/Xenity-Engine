// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(__vita__)

#include <psp2/ctrl.h>
#include "../common/debugScreen.h"
#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include <psp2/touch.h>
#include <cstring>

SceCtrlData ctrl;
SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
	keyMap[SCE_CTRL_CROSS] = &inputs[(int)KeyCode::CROSS];
	keyMap[SCE_CTRL_CIRCLE] = &inputs[(int)KeyCode::CIRCLE];
	keyMap[SCE_CTRL_SQUARE] = &inputs[(int)KeyCode::SQUARE];
	keyMap[SCE_CTRL_TRIANGLE] = &inputs[(int)KeyCode::TRIANGLE];

	keyMap[SCE_CTRL_START] = &inputs[(int)KeyCode::START];
	keyMap[SCE_CTRL_SELECT] = &inputs[(int)KeyCode::SELECT];
	keyMap[SCE_CTRL_LTRIGGER] = &inputs[(int)KeyCode::LTRIGGER1];
	keyMap[SCE_CTRL_RTRIGGER] = &inputs[(int)KeyCode::RTRIGGER1];
	keyMap[SCE_CTRL_RIGHT] = &inputs[(int)KeyCode::RIGHT];
	keyMap[SCE_CTRL_LEFT] = &inputs[(int)KeyCode::DPAD_LEFT];
	keyMap[SCE_CTRL_DOWN] = &inputs[(int)KeyCode::DPAD_DOWN];
	keyMap[SCE_CTRL_UP] = &inputs[(int)KeyCode::DPAD_UP];
}

void CrossInputsInit()
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();
	sceCtrlPeekBufferPositive(0, &ctrl, 1);
	pad.buttons = ctrl.buttons;

	// Left joystick
	pad.lx = ((ctrl.lx - 128) / 256.0f) * 2;
	pad.ly = ((ctrl.ly - 128) / 256.0f) * 2;

	// Right joystick
	pad.rx = ((ctrl.rx - 128) / 256.0f) * 2;
	pad.ry = ((ctrl.ry - 128) / 256.0f) * 2;


	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	std::vector<TouchRaw> touchesRaw;
	memset(touch, 0, sizeof(SceTouchData) * 2);
	for (int screen = 0; screen < SCE_TOUCH_PORT_MAX_NUM; screen++)
	{
		if (sceTouchPeek(screen, &touch[screen], 1) == 1)
		{
			for (int finger = 0; finger < touch[screen].reportNum; finger++)
			{
				TouchRaw t = TouchRaw();
				t.position.x = touch[screen].report[finger].x;
				t.position.y = touch[screen].report[finger].y;
				t.fingerId = touch[screen].report[finger].id;
				t.force = touch[screen].report[finger].force;
				t.screenIndex = screen;
				touchesRaw.push_back(t);
			}
		}
	}
	return touchesRaw;
}

#endif