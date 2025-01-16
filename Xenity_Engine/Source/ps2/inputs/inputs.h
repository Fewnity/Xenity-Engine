// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(_EE)

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

class Input;
class Touch;
class TouchRaw;

void CrossAddInputs(std::map<int, Input*>& s_keyMap, std::map<int, Input*>& s_buttonMap, Input *inputs);
void CrossInputsInit();
InputPad CrossGetInputPad();
std::vector<TouchRaw> CrossUpdateTouch();

#endif