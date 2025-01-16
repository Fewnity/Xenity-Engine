// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <cmath>

#include <engine/tools/math.h>
#include "vector3.h"
#include "vector2_int.h"
#include "vector2.h"

#pragma region Constructors / Destructor

ReflectiveData Vector3::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	return reflectedVariables;
}

Vector3::Vector3(const glm::vec3& glmVec3)
{
	XASSERT(!std::isnan(glmVec3.x), "x is Nan");
	XASSERT(!std::isnan(glmVec3.y), "y is Nan");
	XASSERT(!std::isnan(glmVec3.z), "z is Nan");

	XASSERT(!std::isinf(glmVec3.x), "x is Inf");
	XASSERT(!std::isinf(glmVec3.y), "y is Inf");
	XASSERT(!std::isinf(glmVec3.z), "z is Inf");

	x = glmVec3.x;
	y = glmVec3.y;
	z = glmVec3.z;
}

Vector3::Vector3(const glm::vec4& glmVec4)
{
	XASSERT(!std::isnan(glmVec4.x), "x is Nan");
	XASSERT(!std::isnan(glmVec4.y), "y is Nan");
	XASSERT(!std::isnan(glmVec4.z), "z is Nan");

	XASSERT(!std::isinf(glmVec4.x), "x is Inf");
	XASSERT(!std::isinf(glmVec4.y), "y is Inf");
	XASSERT(!std::isinf(glmVec4.z), "z is Inf");

	x = glmVec4.x;
	y = glmVec4.y;
	z = glmVec4.z;
}

Vector3::Vector3(const float fillValue)
{
	XASSERT(!std::isnan(fillValue), "fillValue is Nan");

	XASSERT(!std::isinf(fillValue), "fillValue is Inf");

	x = fillValue;
	y = fillValue;
	z = fillValue;
}

Vector3::Vector3(const Vector2Int& vect)
{
	x = (float)vect.x;
	y = (float)vect.y;
	z = 0;
}

Vector3::Vector3(const Vector2& vect)
{
	x = vect.x;
	y = vect.y;
	z = 0;
}

#pragma endregion

Vector3 Vector3::LookAt(const Vector3& from, const Vector3& to)
{
	const float xdis = to.x - from.x;
	const float ydis = to.y - from.y;
	const float zdis = to.z - from.z;
	const float xzdis = sqrtf(xdis * xdis + zdis * zdis);

	return Vector3((-atan2f(ydis, xzdis)) * 180 / Math::PI, (-(atan2f(-xdis, zdis))) * 180 / Math::PI, 0);
}

Vector3 Vector3::Normalized() const
{
	const float ls = x * x + y * y + z * z;
	if (ls != 0)
	{
		const float length = sqrtf(ls);
		return Vector3(x / length, y / length, z / length);
	}
	else
	{
		return Vector3(0, 0, 0);
	}
}

Vector3 Vector3::Normalize()
{
	*(this) = Normalized();
	return *(this);
}

float Vector3::MagnitudeSquared() const
{
	return powf(x, 2) + powf(y, 2) + powf(z, 2);
}

float Vector3::Magnitude() const
{
	return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
}

float Vector3::Dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

bool Vector3::HasInvalidValues() const
{
	if (std::isnan(x) || std::isnan(y) || std::isnan(z) ||
		std::isinf(x) || std::isinf(y) || std::isinf(z))
	{
		XASSERT(false, "The Vector3 has invalid values");
		return true;
	}
	return false;
}

float Vector3::Dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, const float t)
{
	return a + (b-a) * t;
}

std::string Vector3::ToString() const
{
	return "{x:" + std::to_string(x) + " y:" + std::to_string(y) + " z:" + std::to_string(z) + "}";
}