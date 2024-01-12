#pragma once
#include <engine/api.h>

#if defined(_WIN32) || defined(_WIN64)

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

struct Input;
struct Touch;
struct TouchRaw;

API void CrossAddInputs(std::map<int, Input*>& keyMap, Input* inputs);
API void CrossInputsInit();
API InputPad CrossGetInputPad();
API std::vector<TouchRaw> CrossUpdateTouch();

#endif