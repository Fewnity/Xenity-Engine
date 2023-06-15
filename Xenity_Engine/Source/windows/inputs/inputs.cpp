#include "inputs.h"

//#ifdef __PSP__

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"


void CrossAddInputs(std::map<int, Input *> &keyMap, Input *inputs)
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

//#endif