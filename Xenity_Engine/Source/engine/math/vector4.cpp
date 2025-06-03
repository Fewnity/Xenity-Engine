// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "vector4.h"
#include "vector3.h"
#include "vector2_int.h"
#include "vector2.h"

#include <cmath>
#include <engine/assertions/assertions.h>

#pragma region Constructors / Destructor

ReflectiveData Vector4::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	Reflective::AddVariable(reflectedVariables, w, "w", true);
	return reflectedVariables;
}

Vector4::Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vector4::Vector4(const float _x, const float _y, const float _z, const float _w)
{
	XASSERT(!std::isnan(_x), "x is Nan");
	XASSERT(!std::isnan(_y), "y is Nan");
	XASSERT(!std::isnan(_z), "z is Nan");
	XASSERT(!std::isnan(_w), "w is Nan");

	XASSERT(!std::isinf(_x), "x is Inf");
	XASSERT(!std::isinf(_y), "y is Inf");
	XASSERT(!std::isinf(_z), "z is Inf");
	XASSERT(!std::isinf(_w), "w is Inf");

	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Vector4::Vector4(const float fillValue)
{
	XASSERT(!std::isnan(fillValue), "fillValue is Nan");

	XASSERT(!std::isinf(fillValue), "fillValue is Inf");

	x = fillValue;
	y = fillValue;
	z = fillValue;
	w = fillValue;
}

Vector4::Vector4(const Vector3& vect)
{
	x = vect.x;
	y = vect.y;
	z = vect.z;
	w = 0; // Default w value for Vector3 to Vector4 conversion
}

Vector4::Vector4(const Vector2& vect)
{
	x = vect.x;
	y = vect.y;
	z = 0; // Default z value for Vector2 to Vector4 conversion
	w = 0; // Default w value for Vector2 to Vector4 conversion
}

Vector4::Vector4(const Vector2Int& vect)
{
	x = static_cast<float>(vect.x);
	y = static_cast<float>(vect.y);
	z = 0; // Default z value for Vector2Int to Vector4 conversion
	w = 0; // Default w value for Vector2Int to Vector4 conversion
}

#pragma endregion

Vector4 Vector4::Normalized() const
{
	const float ls = x * x + y * y + z * z + w * w;
	if (ls != 0)
	{
		const float length = sqrtf(ls);
		return Vector4(x / length, y / length, z / length, w / length);
	}
	else
	{
		return Vector4(0, 0, 0, 0);
	}
}

Vector4 Vector4::Normalize()
{
	*(this) = Normalized();
	return *(this);
}

float Vector4::Magnitude() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Vector4::SquaredMagnitude() const
{
	return x * x + y * y + z * z + w * w;
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, const float t)
{
	return a + (b - a) * t;
}

bool Vector4::HasInvalidValues() const
{
	if (std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w) ||
		std::isinf(x) || std::isinf(y) || std::isinf(z) || std::isinf(w))
	{
		XASSERT(false, "The Vector4 has invalid values");
		return true;
	}
	return false;
}

std::string Vector4::ToString() const
{
	return "{x:" + std::to_string(x) + " y:" + std::to_string(y) + " z:" + std::to_string(z) + " w:" + std::to_string(w) + "}";
}