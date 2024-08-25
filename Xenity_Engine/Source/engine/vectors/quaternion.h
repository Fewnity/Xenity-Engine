// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>

class Vector3;
class Vector4;

class API Quaternion : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;
	Quaternion();

	inline explicit Quaternion(const float x, const float y, const float z, const float w)
		: x(x), y(y), z(z), w(w) {}

	explicit Quaternion(const Vector4& vector);

	static Quaternion Inverse(const Quaternion& q);
	static Quaternion Euler(const float x, const float y, const float z);
	static Quaternion AngleAxis(float angle, const Vector3& axis);
	static Quaternion Identity();
	Vector3 ToEuler() const;

	void Set(const float x, const float y, const float z, const float w);

	/**
	* @brief Return a string representation of the quaternion
	*/
	std::string ToString() const;

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

inline Quaternion Quaternion::Identity()
{
	return Quaternion { 0, 0, 0 , 1 };
}

inline void Quaternion::Set(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline std::string Quaternion::ToString() const
{
	return "{x:" + std::to_string(x) + " y:" + std::to_string(y) + " z:" + std::to_string(z) + " w:" + std::to_string(w) + "}";
}

inline Quaternion operator*(const Quaternion& left, const Quaternion& right)
{
	return Quaternion { left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
				left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z,
				left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x,
				left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z };
}

inline bool operator==(const Quaternion& left, const Quaternion& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

inline bool operator!=(const Quaternion& left, const Quaternion& right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w;
}