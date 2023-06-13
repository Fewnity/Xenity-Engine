#include "inputs.h"

#ifdef __vita__

#include <psp2/ctrl.h>
#include "../common/debugScreen.h"
#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include <psp2/touch.h>
#include <cstring>

SceCtrlData ctrl;
SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

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

    if (pad.lx < 0.2f && pad.lx > -0.2f)
    {
        pad.lx = 0;
    }
    if (pad.ly < 0.2f && pad.ly > -0.2f)
    {
        pad.ly = 0;
    }

    // Right joystick
    pad.rx = ((ctrl.rx - 128) / 256.0f) * 2;
    pad.ry = ((ctrl.ry - 128) / 256.0f) * 2;

    if (pad.rx < 0.2f && pad.rx > -0.2f)
    {
        pad.rx = 0;
    }
    if (pad.ry < 0.2f && pad.ry > -0.2f)
    {
        pad.ry = 0;
    }

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