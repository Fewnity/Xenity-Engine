#include "inputs.h"

#ifdef __vita__

#include <psp2/ctrl.h>
#include "../common/debugScreen.h"
#include "../../engine/inputs/input_system.h"

SceCtrlData ctrl;

void CrossAddInputs(std::map<int, Input *> &keyMap, Input *inputs)
{
    keyMap[SCE_CTRL_CROSS] = &inputs[CROSS];
    keyMap[SCE_CTRL_CIRCLE] = &inputs[CIRCLE];
    keyMap[SCE_CTRL_SQUARE] = &inputs[SQUARE];
    keyMap[SCE_CTRL_TRIANGLE] = &inputs[TRIANGLE];

    keyMap[SCE_CTRL_START] = &inputs[START];
    keyMap[SCE_CTRL_SELECT] = &inputs[SELECT];
    keyMap[SCE_CTRL_LTRIGGER] = &inputs[LTRIGGER1];
    keyMap[SCE_CTRL_RTRIGGER] = &inputs[RTRIGGER1];
    keyMap[SCE_CTRL_RIGHT] = &inputs[RIGHT];
    keyMap[SCE_CTRL_LEFT] = &inputs[LEFT];
    keyMap[SCE_CTRL_DOWN] = &inputs[DOWN];
    keyMap[SCE_CTRL_UP] = &inputs[UP];
}

void CrossInputsInit()
{
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}

InputPad CrossGetInputPad()
{
    InputPad pad = InputPad();
    sceCtrlPeekBufferPositive(0, &ctrl, 1);
    pad.buttons = ctrl.buttons;
    // pad.lx = ctrl.lx;
    // pad.ly = ctrl.ly;
    // pad.rx = ctrl.rx;
    // pad.ry = ctrl.ry;

    // Left joystick
    pad.lx = ((ctrl.lx - 128) / 256.0) * 2;
    pad.ly = ((ctrl.ly - 128) / 256.0) * 2;

    if (pad.lx < 0.2 && pad.lx > -0.2)
    {
        pad.lx = 0;
    }
    if (pad.ly < 0.2 && pad.ly > -0.2)
    {
        pad.ly = 0;
    }

    // Right joystick
    pad.rx = ((ctrl.rx - 128) / 256.0) * 2;
    pad.ry = ((ctrl.ry - 128) / 256.0) * 2;

    if (pad.rx < 0.2 && pad.rx > -0.2)
    {
        pad.rx = 0;
    }
    if (pad.ry < 0.2 && pad.ry > -0.2)
    {
        pad.ry = 0;
    }

    return pad;
}

#endif