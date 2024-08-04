// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
}

Quaternion::Quaternion(const Vector4& vector)
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = vector.w;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
	float normSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	return Quaternion(-q.x / normSq, -q.y / normSq, -q.z / normSq, q.w / normSq);
	//return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Quaternion Quaternion::Euler(const float x, const float y, const float z)
{
	glm::quat q4 = glm::quat(glm::vec3(z / 180.0f * Math::PI, x / 180.0f * Math::PI, y / 180.0f * Math::PI));


	Quaternion quat;
	quat.x = q4.y;
	quat.y = q4.z;
	quat.z = q4.x;
	quat.w = q4.w;
	return quat;
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
{
	float rad = angle * Math::PI / 180.0f;
	float s = sin(rad / 2);
	float c = cos(rad / 2);

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

	glm::vec3 euler = glm::degrees(glm::eulerAngles(glm::quat(w, y, x, z)));
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