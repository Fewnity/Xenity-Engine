#include "inputs.h"

#if defined(__PSP__)

#include <pspctrl.h>
#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"

#define JOYSTICK_DEAD_ZONE 0.25f

SceCtrlData ctrl;

void CrossAddInputs(std::map<int, Input*>& keyMap, Input* inputs)
{
	keyMap[PSP_CTRL_CROSS] = &inputs[CROSS];
	keyMap[PSP_CTRL_CIRCLE] = &inputs[CIRCLE];
	keyMap[PSP_CTRL_SQUARE] = &inputs[SQUARE];
	keyMap[PSP_CTRL_TRIANGLE] = &inputs[TRIANGLE];

	keyMap[PSP_CTRL_START] = &inputs[START];
	keyMap[PSP_CTRL_SELECT] = &inputs[SELECT];
	keyMap[PSP_CTRL_LTRIGGER] = &inputs[LTRIGGER1];
	keyMap[PSP_CTRL_RTRIGGER] = &inputs[RTRIGGER1];
	keyMap[PSP_CTRL_RIGHT] = &inputs[RIGHT];
	keyMap[PSP_CTRL_LEFT] = &inputs[LEFT];
	keyMap[PSP_CTRL_DOWN] = &inputs[DOWN];
	keyMap[PSP_CTRL_UP] = &inputs[UP];
}

void CrossInputsInit()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();
	sceCtrlReadBufferPositive(&ctrl, 1);
	pad.buttons = ctrl.Buttons;

	pad.lx = ((ctrl.Lx - 128) / 256.0f) * 2;
	pad.ly = ((ctrl.Ly - 128) / 256.0f) * 2;

	if (pad.lx <= JOYSTICK_DEAD_ZONE && pad.lx >= -JOYSTICK_DEAD_ZONE)
	{
		pad.lx = 0;
	}
	if (pad.ly <= JOYSTICK_DEAD_ZONE && pad.ly >= -JOYSTICK_DEAD_ZONE)
	{
		pad.ly = 0;
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