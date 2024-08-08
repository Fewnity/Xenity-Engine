// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>

/*
* @brief Contains 4D coordinates
*/
class API Vector4 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector4();
	explicit Vector4(const float x, const float y, const float z, const float w);
	explicit Vector4(const float fillValue);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static Vector4 Lerp(const Vector4& a, const Vector4& b, const float t);

	/**
	* @brief Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector4 Normalized() const;

	/**
	* @brief Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector4 Normalize();

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	/**
	* @brief Return True is the vector has invalid values (NaN or Inf)
	*/
	bool HasInvalidValues() const;

	/**
	* @brief Return a string representation of the vector
	*/
	std::string ToString() const;

	float x;
	float y;
	float z;
	float w;
};

inline Vector4 operator+(const Vector4& left, const Vector4& right)
{
	return Vector4{ left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
}

inline Vector4 operator-(const Vector4& left, const Vector4& right)
{
	return Vector4{ left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}

inline Vector4 operator*(const float value, const Vector4& vec)
{
	return Vector4{ vec.x * value, vec.y * value, vec.z * value, vec.w * value };
}

inline Vector4 operator*(const Vector4& left, const Vector4& right)
{
	return Vector4{ left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w };
}

inline Vector4 operator*(const Vector4& vec, const float value)
{
	return Vector4{ vec.x * value, vec.y * value, vec.z * value, vec.w * value };
}

inline Vector4 operator/(const float value, const Vector4& vec)
{
	return Vector4{ vec.x / value, vec.y / value, vec.z / value, vec.w / value };
}

inline Vector4 operator/(const Vector4& vec, const float value)
{
	return Vector4{ vec.x / value, vec.y / value, vec.z / value, vec.w / value };
}

inline Vector4 operator/(const Vector4& left, const Vector4& right)
{
	return Vector4{ left.x / right.x, left.y / right.y, left.z / right.z, left.w / right.w };
}

inline Vector4& operator/=(Vector4& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	vec.w /= value;
	return vec;
}

inline Vector4& operator*=(Vector4& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	vec.w *= value;
	return vec;
}

inline Vector4& operator+=(Vector4& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	vec.w += value;
	return vec;
}

inline Vector4& operator-=(Vector4& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	vec.z -= value;
	vec.w -= value;
	return vec;
}

inline Vector4& operator/=(Vector4& vec, const Vector4& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	vec.z /= vecRight.z;
	vec.w /= vecRight.w;
	return vec;
}

inline Vector4& operator*=(Vector4& vec, const Vector4& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	vec.z *= vecRight.z;
	vec.w *= vecRight.w;
	return vec;
}

inline Vector4& operator+=(Vector4& vec, const Vector4& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	vec.z += vecRight.z;
	vec.w += vecRight.w;
	return vec;
}

inline Vector4& operator-=(Vector4& vec, const Vector4& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	vec.z -= vecRight.z;
	vec.w -= vecRight.w;
	return vec;
}

inline bool operator==(const Vector4& left, const Vector4& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

inline bool operator!=(const Vector4& left, const Vector4& right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w;
}