#pragma once

#ifdef __PSP__

#include <map>
#include <string>

#include "../../engine/inputs/input_pad.h"

class Input;

void CrossAddInputs(std::map<int, Input *> &keyMap, Input *inputs);
void CrossInputsInit();
InputPad CrossGetInputPad();

#endif