// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <algorithm>
#include <cmath>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <engine/api.h>
#include <engine/reflection/reflection.h>
#include <engine/assertions/assertions.h>

class Vector2Int;
class Vector2;
class Quaternion;

/*
* @brief Contains 3D coordinates
*/
class API Vector3 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector3() : x(0), y(0), z(0) {}

	inline explicit Vector3(const float x, const float y, const float z)
		: x(x), y(y), z(z) 
	{
		XASSERT(!std::isnan(x), "x is Nan");
		XASSERT(!std::isnan(y), "y is Nan");
		XASSERT(!std::isnan(z), "z is Nan");

		XASSERT(!std::isinf(x), "x is Inf");
		XASSERT(!std::isinf(y), "y is Inf");
		XASSERT(!std::isinf(z), "z is Inf");
	}
	Vector3(const glm::vec3& glmVec3);
	Vector3(const glm::vec4& glmVec4);
	explicit Vector3(const float fillValue);
	Vector3(const Vector2Int& vect);
	Vector3(const Vector2& vect);

	float Max() const
	{
		return std::max(std::max(x, y), z);
	}

	float Min() const
	{
		return std::min(std::min(x, y), z);
	}

	/**
	* @brief Get the look rotation in degrees between two vectors
	*/
	static Vector3 LookAt(const Vector3& from, const Vector3& to);

	/**
	* @brief Distance between two vectors
	*/
	static float Distance(const Vector3& a, const Vector3& b);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static Vector3 Lerp(const Vector3& a, const Vector3& b, const float t);

	/**
	* @brief Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector3 Normalized() const;

	/**
	* @brief Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector3 Normalize();

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	float MagnitudeSquared() const;

	/**
	* @brief Get the dot product of this vector
	*/
	float Dot(const Vector3& v) const;

	/**
	* @brief Get the cross product of this vector and another
	*/
	Vector3 Cross(const Vector3& v) const;

	/**
	* @brief Return True is the vector has invalid values (NaN or Inf)
	*/
	bool HasInvalidValues() const;

	/**
	* @brief Get the dot product of two vectors
	*/
	static float Dot(const Vector3& a, const Vector3& b);

	/**
	* @brief Get the cross product of two vectors
	*/
	static Vector3 Cross(const Vector3& a, const Vector3& b);
	
	/**
	* @brief Return a string representation of the vector
	*/
	std::string ToString() const;

	float x;
	float y;
	float z;
};

inline float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	const float xDis = a.x - b.x;
	const float yDis = a.y - b.y;
	const float zDis = a.z - b.z;
	return sqrtf(xDis * xDis + yDis * yDis + zDis * zDis);
}

inline Vector3 operator+(const Vector3& left, const Vector3& right)
{
	return Vector3{ left.x + right.x, left.y + right.y, left.z + right.z };
}

inline Vector3 operator-(const Vector3& left, const Vector3& right)
{
	return Vector3{ left.x - right.x, left.y - right.y, left.z - right.z };
}

inline Vector3 operator-(const Vector3& vec)
{
	return Vector3{ -vec.x, -vec.y, -vec.z };
}

inline Vector3 operator*(float value, const Vector3& vec)
{
	return Vector3{ vec.x * value, vec.y * value, vec.z * value };
}

inline Vector3 operator*(const Vector3& left, const Vector3& right)
{
	return Vector3{ left.x * right.x, left.y * right.y, left.z * right.z };
}

inline Vector3 operator*(const Vector3& vec, const float value)
{
	return Vector3{ vec.x * value, vec.y * value, vec.z * value };
}

inline Vector3 operator/(float value, const Vector3& vec)
{
	return Vector3{ vec.x / value, vec.y / value, vec.z / value };
}

inline Vector3 operator/(const Vector3& vec, const float value)
{
	return Vector3{ vec.x / value, vec.y / value, vec.z / value };
}

inline Vector3 operator/(const Vector3& left, const Vector3& right)
{
	return Vector3{ left.x / right.x, left.y / right.y, left.z / right.z };
}

inline Vector3& operator/=(Vector3& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	return vec;
}

inline Vector3& operator*=(Vector3& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	return vec;
}

inline Vector3& operator+=(Vector3& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	return vec;
}

inline Vector3& operator-=(Vector3& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	vec.z -= value;
	return vec;
}

inline Vector3& operator/=(Vector3& vec, const Vector3& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	vec.z /= vecRight.z;
	return vec;
}

inline Vector3& operator*=(Vector3& vec, const Vector3& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	vec.z *= vecRight.z;
	return vec;
}

inline Vector3& operator+=(Vector3& vec, const Vector3& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	vec.z += vecRight.z;
	return vec;
}

inline Vector3& operator-=(Vector3& vec, const Vector3& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	vec.z -= vecRight.z;
	return vec;
}

inline bool operator==(const Vector3& left, const Vector3& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z;
}

inline bool operator!=(const Vector3& left, const Vector3& right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z;
}

Vector3 operator*(const Quaternion& left, const Vector3& right);