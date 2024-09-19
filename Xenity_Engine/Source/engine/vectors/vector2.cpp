// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "vector2.h"
#include "vector2_int.h"
#include "vector3.h"

#include <cmath>

#pragma region Constructors

ReflectiveData Vector2::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	return reflectedVariables;
}

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(const float _x, const float _y)
{
	XASSERT(!std::isnan(_x), "x is Nan");
	XASSERT(!std::isnan(_y), "y is Nan");

	XASSERT(!std::isinf(_x), "x is Inf");
	XASSERT(!std::isinf(_y), "y is Inf");

	x = _x;
	y = _y;
}

Vector2::Vector2(const float fillValue)
{
	XASSERT(!std::isnan(fillValue), "fillValue is Nan");

	XASSERT(!std::isinf(fillValue), "fillValue is Inf");

	x = fillValue;
	y = fillValue;
}

Vector2::Vector2(const Vector3& vect3)
{
	x = vect3.x;
	y = vect3.y;
}

Vector2::Vector2(const Vector2Int& vect2Int)
{
	x = (float)vect2Int.x;
	y = (float)vect2Int.y;
}

#pragma endregion

// From https://github.com/microsoft/referencesource/blob/5697c29004a34d80acdaf5742d7e699022c64ecd/System.Numerics/System/Numerics/Vector2.cs
Vector2 Vector2::Normalized() const
{
	const float ls = this->x * this->x + this->y * this->y;
	float invNorm = 0;
	if (ls != 0)
		invNorm = 1.0f / sqrtf(ls);

	return Vector2(this->x * invNorm, this->y * invNorm);
}

Vector2 Vector2::Normalize()
{
	*(this) = Normalized();
	return *(this);
}

float Vector2::Magnitude() const
{
	return sqrtf(powf(x, 2) + powf(y, 2));
}

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
	const float xDis = a.x - b.x;
	const float yDis = a.y - b.y;
	return sqrtf(xDis * xDis + yDis * yDis);
}

Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, const float t)
{
	return a + (b - a) * t;
}

bool Vector2::HasInvalidValues() const
{
	if (std::isnan(x) || std::isnan(y) ||
		std::isinf(x) || std::isinf(y))
	{
		XASSERT(false, "The Vector2 has invalid values");
		return true;
	}
	return false;
}

std::string Vector2::ToString() const
{
	return "{x:" + std::to_string(x) + " y:" + std::to_string(y) + "}";
}
