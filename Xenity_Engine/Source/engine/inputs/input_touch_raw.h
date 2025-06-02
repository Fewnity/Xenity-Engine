// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/math/vector2_int.h>

struct TouchRaw
{
	Vector2Int position = Vector2Int(0);
	int force = 0;
	int fingerId = 0;
	int screenIndex = 0;
};