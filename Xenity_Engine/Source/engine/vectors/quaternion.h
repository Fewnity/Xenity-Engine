// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>
#include <cmath>

class Vector3;
class Vector4;

class API Quaternion : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;
	Quaternion();

	inline explicit Quaternion(const float x, const float y, const float z, const float w)
		: x(x), y(y), z(z), w(w) 
	{
		Normalize();
	}

	explicit Quaternion(const Vector4& vector);

	[[nodiscard]] static Quaternion Inverse(const Quaternion& q);
	[[nodiscard]] static Quaternion Euler(const float x, const float y, const float z);

	/**
	* @brief Linearly interpolates between quaternions
	*/
	[[nodiscard]] static Quaternion Lerp(const Quaternion& a, const Quaternion& b, const float t);
	[[nodiscard]] static float Dot(const Quaternion& q1, const Quaternion& q2);
	[[nodiscard]] static Quaternion AngleAxis(float angle, const Vector3& axis);
	[[nodiscard]] static Quaternion Identity();
	[[nodiscard]] Vector3 ToEuler() const;

	void Set(const float x, const float y, const float z, const float w);
	void Normalize();
	Quaternion Normalized();

	[[nodiscard]] Vector3 GetForward() const;

	[[nodiscard]] Vector3 GetUp() const;

	[[nodiscard]] Vector3 GetRight() const;

	/**
	* @brief Return a string representation of the quaternion
	*/
	[[nodiscard]] std::string ToString() const;

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

inline Quaternion Quaternion::Identity()
{
	return Quaternion { 0, 0, 0, 1 };
}

inline void Quaternion::Set(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	Normalize();
}

inline void Quaternion::Normalize()
{
	const float length = sqrtf(x * x + y * y + z * z + w * w);
	if (length > 0.0f)
	{
		const float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
}

inline Quaternion Quaternion::Normalized()
{
	Quaternion normalizedQuaternion = Quaternion(x, y, z, w);
	normalizedQuaternion.Normalize();
	return normalizedQuaternion;
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