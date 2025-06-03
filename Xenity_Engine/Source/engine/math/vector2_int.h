// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>

class Vector3;
class Vector2;

/*
* @brief Contains integer 2D coordinates 
*/
class API Vector2Int : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector2Int();
	explicit Vector2Int(const int x, const int y);
	explicit Vector2Int(const int fillValue);
	explicit Vector2Int(const Vector3& vect);
	explicit Vector2Int(const Vector2& vect);

	/**
	* @brief Distance between two vectors
	*/
	static float Distance(const Vector2Int& a, const Vector2Int& b);

	/**
	* @brief Get the biggest value of the vector
	*/
	int Max() const
	{
		return std::max(x, y);
	}

	/**
	* @brief Get the smallest value of the vector
	*/
	int Min() const
	{
		return std::min(x, y);
	}

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	/**
	* @brief Get the squared length of this vector
	*/
	float SquaredMagnitude() const;

	/**
	* @brief Return a string representation of the vector
	*/
	std::string ToString() const;

	int x;
	int y;
};


inline Vector2Int operator+(const Vector2Int& left, const Vector2Int& right)
{
	return Vector2Int{ (int)(left.x + right.x), (int)(left.y + right.y) };
}

inline Vector2Int operator-(const Vector2Int& left, const Vector2Int& right)
{
	return Vector2Int{ (int)(left.x - right.x), (int)(left.y - right.y) };
}

inline Vector2Int operator-(const Vector2Int& vec)
{
	return Vector2Int{ -vec.x, -vec.y };
}

inline Vector2Int operator*(const float value, const Vector2Int& vec)
{
	return Vector2Int{ (int)(vec.x * value), (int)(vec.y * value) };
}

inline Vector2Int operator*(const Vector2Int& left, const Vector2Int& right)
{
	return Vector2Int{ (int)(left.x * right.x), (int)(left.y * right.y) };
}

inline Vector2Int operator*(const Vector2Int& vec, const float value)
{
	return Vector2Int{ (int)(vec.x * value), (int)(vec.y * value) };
}

inline Vector2Int operator/(const float value, const Vector2Int& vec)
{
	return Vector2Int{ (int)(vec.x / value), (int)(vec.y / value) };
}

inline Vector2Int operator/(const Vector2Int& vec, const float value)
{
	return Vector2Int{ (int)(vec.x / value), (int)(vec.y / value) };
}

inline Vector2Int operator/(const Vector2Int& left, const Vector2Int& right)
{
	return Vector2Int{ (int)(left.x / right.x), (int)(left.y / right.y) };
}

inline Vector2Int& operator/=(Vector2Int& vec, const float value)
{
	vec.x /= (int)value;
	vec.y /= (int)value;
	return vec;
}

inline Vector2Int& operator*=(Vector2Int& vec, const float value)
{
	vec.x *= (int)value;
	vec.y *= (int)value;
	return vec;
}

inline Vector2Int& operator+=(Vector2Int& vec, const float value)
{
	vec.x += (int)value;
	vec.y += (int)value;
	return vec;
}

inline Vector2Int& operator-=(Vector2Int& vec, const float value)
{
	vec.x -= (int)value;
	vec.y -= (int)value;
	return vec;
}

inline Vector2Int& operator/=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	return vec;
}

inline Vector2Int& operator*=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	return vec;
}

inline Vector2Int& operator+=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	return vec;
}

inline Vector2Int& operator-=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	return vec;
}

inline bool operator==(const Vector2Int& left, const Vector2Int& right)
{
	return left.x == right.x && left.y == right.y;
}

inline bool operator!=(const Vector2Int& left, const Vector2Int& right)
{
	return left.x != right.x || left.y != right.y;
}