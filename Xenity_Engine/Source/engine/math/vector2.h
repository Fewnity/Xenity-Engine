// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>

class Vector3;
class Vector2Int;

/*
* @brief Contains 2D coordinates
*/
class API Vector2 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector2();
	explicit Vector2(const float x, const float y);
	explicit Vector2(const float fillValue);
	explicit Vector2(const Vector3& vect);
	explicit Vector2(const Vector2Int& vect);

	/**
	* @brief Distance between two vectors
	*/
	static float Distance(const Vector2& a, const Vector2& b);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static Vector2 Lerp(const Vector2& a, const Vector2& b, const float t);

	/**
	* @brief Get the biggest value of the vector
	*/
	float Max() const
	{
		return std::max(x, y);
	}

	/**
	* @brief Get the smallest value of the vector
	*/
	float Min() const
	{
		return std::min(x, y);
	}

	/**
	* @brief Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector2 Normalized() const;

	/**
	* @brief Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector2 Normalize();

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	/**
	* @brief Get the squared length of this vector
	*/
	float SquaredMagnitude() const;

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
};


inline Vector2 operator+(const Vector2& left, const Vector2& right)
{
	return Vector2{ left.x + right.x, left.y + right.y };
}

inline Vector2 operator-(const Vector2& left, const Vector2& right)
{
	return Vector2{ left.x - right.x, left.y - right.y };
}

inline Vector2 operator-(const Vector2& vec)
{
	return Vector2{ -vec.x, -vec.y };
}

inline Vector2 operator*(const float value, const Vector2& vec)
{
	return Vector2{ vec.x * value, vec.y * value };
}

inline Vector2 operator*(const Vector2& left, const Vector2& right)
{
	return Vector2{ left.x * right.x, left.y * right.y };
}

Vector2 operator*(const Vector2& left, const Vector2Int& right);

Vector2 operator*(const Vector2Int& left, const Vector2& right);

inline Vector2 operator*(const Vector2& vec, const float value)
{
	return Vector2{ vec.x * value, vec.y * value };
}

inline Vector2 operator/(const float value, const Vector2& vec)
{
	return Vector2{ vec.x / value, vec.y / value };
}

inline Vector2 operator/(const Vector2& vec, const float value)
{
	return Vector2{ vec.x / value, vec.y / value };
}

inline Vector2 operator/(const Vector2& left, const Vector2& right)
{
	return Vector2{ left.x / right.x, left.y / right.y };
}

inline Vector2& operator/=(Vector2& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	return vec;
}

inline Vector2& operator*=(Vector2& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	return vec;
}

inline Vector2& operator+=(Vector2& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	return vec;
}

inline Vector2& operator-=(Vector2& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	return vec;
}

inline Vector2& operator/=(Vector2& vec, const Vector2& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	return vec;
}

inline Vector2& operator*=(Vector2& vec, const Vector2& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	return vec;
}

inline Vector2& operator+=(Vector2& vec, const Vector2& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	return vec;
}

inline Vector2& operator-=(Vector2& vec, const Vector2& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	return vec;
}

inline bool operator==(const Vector2& left, const Vector2& right)
{
	return left.x == right.x && left.y == right.y;
}

inline bool operator!=(const Vector2& left, const Vector2& right)
{
	return left.x != right.x || left.y != right.y;
}