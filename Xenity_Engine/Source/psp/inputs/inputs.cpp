#include "inputs.h"

#ifdef __PSP__

#include <pspctrl.h>
#include "../../engine/inputs/input_system.h"
#include "../../engine/debug/debug.h"

SceCtrlData ctrl;

void CrossAddInputs(std::map<int, Input *> &keyMap, Input *inputs)
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
    // sceCtrlPeekBufferPositive(0, &ctrl, 1);
    sceCtrlReadBufferPositive(&ctrl, 1);
    pad.buttons = ctrl.Buttons;
    pad.lx = ctrl.Lx;
    pad.ly = ctrl.Ly;
    return pad;
}

#endif