// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "assertions.h"

#include <engine/debug/debug.h>

void OnAssertionFailed(const std::string& condition, const std::string& message, const std::string fileName, size_t line)
{
	Debug::PrintError("Assertion failed!\nCondition: " + condition + "\nMessage: " + message + "\nFile: " + fileName + "\nLine: " + std::to_string(line));
}
