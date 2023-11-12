#include "inputs.h"

#if defined(_EE)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"

#define JOYSTICK_DEAD_ZONE 0.25f

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

#endif