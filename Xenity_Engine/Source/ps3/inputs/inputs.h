// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(__PS3__)

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

class Input;
class Touch;
class TouchRaw;

void CrossAddInputs(std::map<int, Input*>& s_keyMap, std::map<int, Input*>& s_buttonMap, Input *inputs);
void CrossInputsInit();
InputPad CrossGetInputPad(const int controllerId);
std::vector<TouchRaw> CrossUpdateTouch();

#endif