// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "vector2_int.h"
#include "vector3.h"
#include "vector2.h"

#include <cmath>

#pragma region Constructors

ReflectiveData Vector2Int::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	return reflectedVariables;
}

Vector2Int::Vector2Int()
{
	x = 0;
	y = 0;
}

Vector2Int::Vector2Int(const int _x, const int _y)
{
	x = _x;
	y = _y;
}

Vector2Int::Vector2Int(const int fillValue)
{
	x = fillValue;
	y = fillValue;
}

Vector2Int::Vector2Int(const Vector3& vect)
{
	x = static_cast<int>(vect.x);
	y = static_cast<int>(vect.y);
}

Vector2Int::Vector2Int(const Vector2& vect)
{
	x = static_cast<int>(vect.x);
	y = static_cast<int>(vect.y);
}

#pragma endregion

float Vector2Int::Magnitude() const
{
	return sqrtf(powf((float)x, 2) + powf((float)y, 2));
}

float Vector2Int::Distance(const Vector2Int& a, const Vector2Int& b)
{
	const float xDis = (float)(a.x - b.x);
	const float yDis = (float)(a.y - b.y);
	return sqrtf(xDis * xDis + yDis * yDis);
}

std::string Vector2Int::ToString() const
{
	return "{x:" + std::to_string(x) + " y:" + std::to_string(y) + "}";
}