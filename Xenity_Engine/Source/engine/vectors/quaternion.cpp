// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "quaternion.h"

#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>

#include <engine/tools/math.h>
#include "vector3.h"
#include "vector4.h"

Quaternion::Quaternion()
{
	// Set identity
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

Quaternion::Quaternion(const Vector4& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	w = vector.w;
	Normalize();
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
	float normSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	return Quaternion(-q.x / normSq, -q.y / normSq, -q.z / normSq, q.w / normSq);
	//return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Vector3 Quaternion::GetForward() const
{
	return Vector3(
		2 * (x * z + w * y),
		2 * (y * z - w * x),
		1 - 2 * (x * x + y * y)
	);
}

Vector3 Quaternion::GetUp() const
{
	return Vector3(
		2 * (x * y - w * z),
		1 - 2 * (x * x + z * z),
		2 * (y * z + w * x)
	);
}

Vector3 Quaternion::GetRight() const
{
	return Vector3(
		1 - 2 * (y * y + z * z),
		2 * (x * y + w * z),
		2 * (x * z - w * y)
	);
}

Quaternion Quaternion::Euler(const float x, const float y, const float z)
{
	XASSERT(!std::isnan(x), "x is Nan");
	XASSERT(!std::isnan(y), "y is Nan");
	XASSERT(!std::isnan(z), "z is Nan");

	XASSERT(!std::isinf(x), "x is Inf");
	XASSERT(!std::isinf(y), "y is Inf");
	XASSERT(!std::isinf(z), "z is Inf");

	const glm::quat q4 = glm::quat(glm::vec3(z / 180.0f * Math::PI, x / 180.0f * Math::PI, y / 180.0f * Math::PI));

	Quaternion quat;
	quat.x = q4.y;
	quat.y = q4.z;
	quat.z = q4.x;
	quat.w = q4.w;
	return quat;
}

float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) 
{
	return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, const float t)
{
	Quaternion q2Corrected = q2;

	if (Dot(q1, q2) < 0.0f) {
		q2Corrected.w = -q2.w;
		q2Corrected.x = -q2.x;
		q2Corrected.y = -q2.y;
		q2Corrected.z = -q2.z;
	}

	// Interpolation lin�aire
	Quaternion result;
	result.x = q1.x - t * (q1.x - q2Corrected.x);
	result.y = q1.y - t * (q1.y - q2Corrected.y);
	result.z = q1.z - t * (q1.z - q2Corrected.z);
	result.w = q1.w - t * (q1.w - q2Corrected.w);

	result.Normalize();
	return result;
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
{
	const float rad = angle * Math::PI / 180.0f;
	const float s = std::sin(rad / 2);
	const float c = std::cos(rad / 2);

	return Quaternion(axis.x * s, axis.y * s, axis.z * s, c);
}

Vector3 Quaternion::ToEuler() const
{
	// Check if this code is faster
	//const glm::mat4 matChildRelative = glm::mat4_cast(glm::quat(q.w, q.x, q.y, q.z));

	//float x, y, z;
	//glm::extractEulerAngleYXZ(matChildRelative, y, x, z);
	//x = glm::degrees(x);
	//y = glm::degrees(y);
	//z = glm::degrees(z);

	const glm::vec3 euler = glm::degrees(glm::eulerAngles(glm::quat(w, y, x, z)));
	return Vector3(euler.y, euler.x, euler.z);
}

ReflectiveData Quaternion::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	Reflective::AddVariable(reflectedVariables, w, "w", true);
	return reflectedVariables;
}