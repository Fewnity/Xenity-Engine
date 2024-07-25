// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>

#include <engine/reflection/reflection.h>

class API Quaternion : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;
	Quaternion();
	Quaternion(const float x, const float y, const float z, const float w);

	static Quaternion Inverse(Quaternion q);
	static Quaternion Euler(const float x, const float y, const float z);
	static Quaternion Identity();

	float x;
	float y;
	float z;
	float w;
};

inline Quaternion Quaternion::Identity()
{
	return { 0, 0, 0 , 1 };
}

inline Quaternion operator*(const Quaternion& left, const Quaternion& right)
{
	return { left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
				left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z,
				left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x,
				left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z };
}
