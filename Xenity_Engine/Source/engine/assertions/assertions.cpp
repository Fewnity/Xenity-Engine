// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "assertions.h"

#include <engine/debug/debug.h>

void OnAssertionFailed(const std::string& message)
{
	Debug::PrintError("Assertion failed: " + message);
}
