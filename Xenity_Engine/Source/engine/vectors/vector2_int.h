// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/reflection/reflection.h>

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

	/**
	* @brief Linearly interpolates between vectors
	*/
	static float Distance(const Vector2Int& a, const Vector2Int& b);

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

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