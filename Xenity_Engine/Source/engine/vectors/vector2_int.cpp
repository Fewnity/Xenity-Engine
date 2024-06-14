// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "vector2_int.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__) || defined(_EE)
#undef __STRICT_ANSI__
#endif
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
	this->x = 0;
	this->y = 0;
}

Vector2Int::Vector2Int(const int x, const int y)
{
	this->x = x;
	this->y = y;
}

Vector2Int::Vector2Int(const int fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
}

#pragma endregion

float Vector2Int::Magnitude() const
{
	return sqrtf(powf((float)this->x, 2) + powf((float)this->y, 2));
}

float Vector2Int::Distance(const Vector2Int& a, const Vector2Int& b)
{
	const float xDis = (float)(a.x - b.x);
	const float yDis = (float)(a.y - b.y);
	return sqrtf(xDis * xDis + yDis * yDis);
}