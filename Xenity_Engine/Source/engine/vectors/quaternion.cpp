// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "quaternion.h"
#include <glm/ext/quaternion_float.hpp>
#include <engine/tools/math.h>

Quaternion::Quaternion()
{
}

Quaternion::Quaternion(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion Quaternion::Inverse(Quaternion q)
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

ReflectiveData Quaternion::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	Reflective::AddVariable(reflectedVariables, w, "w", true);
	return reflectedVariables;
}