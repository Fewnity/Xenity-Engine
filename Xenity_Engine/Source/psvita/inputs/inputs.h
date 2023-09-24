#pragma once

#ifdef __vita__

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

class Input;
class Touch;
class TouchRaw;

void CrossAddInputs(std::map<int, Input*>& keyMap, Input* inputs);
void CrossInputsInit();
InputPad CrossGetInputPad();
std::vector<TouchRaw> CrossUpdateTouch();

#endif