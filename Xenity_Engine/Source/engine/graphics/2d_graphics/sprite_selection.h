// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/vectors/vector2.h>
#include <engine/reflection/reflection.h>

class SpriteSelection : Reflective
{
public:
	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, position, "position", true);
		Reflective::AddVariable(reflectedVariables, size, "size", true);
		Reflective::AddVariable(reflectedVariables, pivot, "pivot", true);
		return reflectedVariables;
	}

	Vector2 position;
	Vector2 size;
	Vector2 pivot;
};