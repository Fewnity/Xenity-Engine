// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#ifdef __vita__

#include <string>

void PsVitaDebugInit();
void PsVitaDebugPrint(std::string text);

#endif