// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "vector2_int.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__) || defined(_EE)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#pragma region Constructors

ReflectiveData Vector2Int::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	return reflectedVariables;
}

Vector2Int::Vector2Int()
{
	this->x = 0;
	this->y = 0;
}

Vector2Int::Vector2Int(const int x, const int y)
{
	this->x = x;
	this->y = y;
}

Vector2Int::Vector2Int(const int fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
}

#pragma endregion

float Vector2Int::Magnitude() const
{
	return sqrtf(powf((float)this->x, 2) + powf((float)this->y, 2));
}

float Vector2Int::Distance(const Vector2Int& a, const Vector2Int& b)
{
	const float xDis = (float)(a.x - b.x);
	const float yDis = (float)(a.y - b.y);
	return sqrtf(xDis * xDis + yDis * yDis);
}

#pragma region Operators

Vector2Int operator+(const Vector2Int& left, const Vector2Int& right)
{
	return { (int)(left.x + right.x), (int)(left.y + right.y) };
}

Vector2Int operator-(const Vector2Int& left, const Vector2Int& right)
{
	return { (int)(left.x - right.x), (int)(left.y - right.y) };
}

Vector2Int operator*(const float value, const Vector2Int& vec)
{
	return { (int)(vec.x * value), (int)(vec.y * value) };
}

Vector2Int operator*(const Vector2Int& left, const Vector2Int& right)
{
	return { (int)(left.x * right.x), (int)(left.y * right.y) };
}

Vector2Int operator*(const Vector2Int& vec, const float value)
{
	return { (int)(vec.x * value), (int)(vec.y * value) };
}

Vector2Int operator/(const float value, const Vector2Int& vec)
{
	return { (int)(vec.x / value), (int)(vec.y / value) };
}

Vector2Int operator/(const Vector2Int& vec, const float value)
{
	return { (int)(vec.x / value), (int)(vec.y / value) };
}

Vector2Int operator/(const Vector2Int& left, const Vector2Int& right)
{
	return { (int)(left.x / right.x), (int)(left.y / right.y) };
}

Vector2Int operator/=(Vector2Int& vec, const float value)
{
	vec.x /= (int)value;
	vec.y /= (int)value;
	return vec;
}

Vector2Int operator*=(Vector2Int& vec, const float value)
{
	vec.x *= (int)value;
	vec.y *= (int)value;
	return vec;
}

Vector2Int operator+=(Vector2Int& vec, const float value)
{
	vec.x += (int)value;
	vec.y += (int)value;
	return vec;
}

Vector2Int operator-=(Vector2Int& vec, const float value)
{
	vec.x -= (int)value;
	vec.y -= (int)value;
	return vec;
}

Vector2Int operator/=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	return vec;
}

Vector2Int operator*=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	return vec;
}

Vector2Int operator+=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	return vec;
}

Vector2Int operator-=(Vector2Int& vec, const Vector2Int& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	return vec;
}

bool operator==(const Vector2Int& left, const Vector2Int& right)
{
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const Vector2Int& left, const Vector2Int& right)
{
	return left.x != right.x || left.y != right.y;
}

#pragma endregion