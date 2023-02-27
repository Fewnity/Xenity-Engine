#include "vector2.h"
#include <corecrt_math.h>

#pragma region Constructors

Vector2::Vector2() {
	this->x = 0;
	this->y = 0;
}

Vector2::Vector2(const float x, const float y) {
	this->x = x;
	this->y = y;
}

Vector2::Vector2(const float fillValue) {
	this->x = fillValue;
	this->y = fillValue;
}

#pragma endregion

// From https://github.com/microsoft/referencesource/blob/5697c29004a34d80acdaf5742d7e699022c64ecd/System.Numerics/System/Numerics/Vector2.cs
Vector2 Vector2::normalize()
{
	float ls = this->x * this->x + this->y * this->y;
	float invNorm = 0;
	if (ls != 0)
		invNorm = 1.0f / sqrtf(ls);

	return Vector2(this->x * invNorm, this->y * invNorm);
}

float Vector2::magnitude()
{
	return sqrtf(powf(this->x, 2) + powf(this->y, 2));
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
	return { vec.x * value, vec.y * value};
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