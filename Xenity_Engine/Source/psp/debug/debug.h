// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#ifdef __PSP__

#include <string>

void PspDebugInit();
void PspDebugPrint(std::string text);

#endif