#include "vector2.h"
#include "vector2_int.h"
#include "vector3.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#pragma region Constructors

std::unordered_map<std::string, ReflectionEntry> Vector2::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, x, "x", true);
	Reflection::AddReflectionVariable(reflectedVariables, y, "y", true);
	return reflectedVariables;
}

Vector2::Vector2()
{
	this->x = 0;
	this->y = 0;
}

Vector2::Vector2(const float x, const float y)
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2(const float fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
}

Vector2::Vector2(const Vector3& vect3)
{
	this->x = vect3.x;
	this->y = vect3.y;
}

Vector2::Vector2(const Vector2Int& vect2Int)
{
	this->x = (float)vect2Int.x;
	this->y = (float)vect2Int.y;
}

#pragma endregion

// From https://github.com/microsoft/referencesource/blob/5697c29004a34d80acdaf5742d7e699022c64ecd/System.Numerics/System/Numerics/Vector2.cs
Vector2 Vector2::Normalized()
{
	float ls = this->x * this->x + this->y * this->y;
	float invNorm = 0;
	if (ls != 0)
		invNorm = 1.0f / sqrtf(ls);

	return Vector2(this->x * invNorm, this->y * invNorm);
}

float Vector2::Magnitude()
{
	return sqrtf(powf(this->x, 2) + powf(this->y, 2));
}

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
	float xDis = a.x - b.x;
	float yDis = a.y - b.y;
	return sqrtf(xDis * xDis + yDis * yDis);
}

Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, const float t)
{
	return a + (b - a) * t;
}

#pragma region Operators

Vector2 operator+(const Vector2& left, const Vector2& right)
{
	return { left.x + right.x, left.y + right.y };
}

Vector2 operator-(const Vector2& left, const Vector2& right)
{
	return { left.x - right.x, left.y - right.y };
}

Vector2 operator*(const float value, const Vector2& vec)
{
	return { vec.x * value, vec.y * value };
}

Vector2 operator*(const Vector2& left, const Vector2& right)
{
	return { left.x * right.x, left.y * right.y };
}

Vector2 operator*(const Vector2& vec, const float value)
{
	return { vec.x * value, vec.y * value };
}

Vector2 operator/(const float value, const Vector2& vec)
{
	return { vec.x / value, vec.y / value };
}

Vector2 operator/(const Vector2& vec, const float value)
{
	return { vec.x / value, vec.y / value };
}

Vector2 operator/(const Vector2& left, const Vector2& right)
{
	return { left.x / right.x, left.y / right.y };
}

Vector2 operator/=(Vector2& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	return vec;
}

Vector2 operator*=(Vector2& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	return vec;
}

Vector2 operator+=(Vector2& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	return vec;
}

Vector2 operator-=(Vector2& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	return vec;
}

Vector2 operator/=(Vector2& vec, const Vector2& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	return vec;
}

Vector2 operator*=(Vector2& vec, const Vector2& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	return vec;
}

Vector2 operator+=(Vector2& vec, const Vector2& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	return vec;
}

Vector2 operator-=(Vector2& vec, const Vector2& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	return vec;
}

bool operator==(const Vector2& left, const Vector2& right)
{
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const Vector2& left, const Vector2& right)
{
	return left.x != right.x || left.y != right.y;
}

#pragma endregion