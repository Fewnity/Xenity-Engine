// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(__vita__)

#include <psp2/ctrl.h>
#include "../common/debugScreen.h"
#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"
#include "../../engine/ui/screen.h"
#include <psp2/touch.h>
#include <cstring>

SceCtrlData ctrl;
SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];
SceTouchPanelInfo panelInfo[SCE_TOUCH_PORT_MAX_NUM];

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
	buttonMap[SCE_CTRL_CROSS] = &inputs[(int)KeyCode::CROSS];
	buttonMap[SCE_CTRL_CIRCLE] = &inputs[(int)KeyCode::CIRCLE];
	buttonMap[SCE_CTRL_SQUARE] = &inputs[(int)KeyCode::SQUARE];
	buttonMap[SCE_CTRL_TRIANGLE] = &inputs[(int)KeyCode::TRIANGLE];

	buttonMap[SCE_CTRL_START] = &inputs[(int)KeyCode::START];
	buttonMap[SCE_CTRL_SELECT] = &inputs[(int)KeyCode::SELECT];
	buttonMap[SCE_CTRL_LTRIGGER] = &inputs[(int)KeyCode::LTRIGGER1];
	buttonMap[SCE_CTRL_RTRIGGER] = &inputs[(int)KeyCode::RTRIGGER1];
	buttonMap[SCE_CTRL_RIGHT] = &inputs[(int)KeyCode::RIGHT];
	buttonMap[SCE_CTRL_LEFT] = &inputs[(int)KeyCode::DPAD_LEFT];
	buttonMap[SCE_CTRL_DOWN] = &inputs[(int)KeyCode::DPAD_DOWN];
	buttonMap[SCE_CTRL_UP] = &inputs[(int)KeyCode::DPAD_UP];
}

void CrossInputsInit()
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);
	sceTouchGetPanelInfo(SCE_TOUCH_PORT_FRONT, &panelInfo[0]);
	sceTouchGetPanelInfo(SCE_TOUCH_PORT_BACK, &panelInfo[1]);
}

InputPad CrossGetInputPad(const int controllerId)
{
	if (controllerId != 0)
		return;

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

long mapValue(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
				t.position.x = mapValue(touch[screen].report[finger].x, panelInfo[0].minAaX, panelInfo[0].maxAaX, 0, Screen::GetWidth());
				t.position.y = mapValue(touch[screen].report[finger].y, panelInfo[0].minAaY, panelInfo[0].maxAaY, 0, Screen::GetHeight());
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