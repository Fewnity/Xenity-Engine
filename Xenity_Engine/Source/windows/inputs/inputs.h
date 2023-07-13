#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

class Input;
class Touch;
class TouchRaw;

API void CrossAddInputs(std::map<int, Input *> &keyMap, Input *inputs);
API void CrossInputsInit();
API InputPad CrossGetInputPad();
API std::vector<TouchRaw> CrossUpdateTouch();

#endif