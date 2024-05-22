// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(__vita__)

#include <map>
#include <string>
#include <vector>
#include "../../engine/inputs/input_pad.h"

class Input;
class Touch;
class TouchRaw;

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs);
void CrossInputsInit();
InputPad CrossGetInputPad();
std::vector<TouchRaw> CrossUpdateTouch();

#endif