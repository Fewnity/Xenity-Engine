#include "vector2_int.h"
#include <corecrt_math.h>

#pragma region Constructors

Vector2Int::Vector2Int() {
	this->x = 0;
	this->y = 0;
}

Vector2Int::Vector2Int(const int x, const int y) {
	this->x = x;
	this->y = y;
}

Vector2Int::Vector2Int(const int fillValue) {
	this->x = fillValue;
	this->y = fillValue;
}

#pragma endregion

// From https://github.com/microsoft/referencesource/blob/5697c29004a34d80acdaf5742d7e699022c64ecd/System.Numerics/System/Numerics/Vector2Int.cs
Vector2Int Vector2Int::normalize()
{
	float ls = this->x * this->x + this->y * this->y;
	float invNorm = 0;
	if (ls != 0)
		invNorm = 1.0f / sqrtf(ls);

	return Vector2Int(this->x * invNorm, this->y * invNorm);
}

float Vector2Int::magnitude()
{
	return sqrtf(powf(this->x, 2) + powf(this->y, 2));
}

float Vector2Int::Distance(const Vector2Int a, const Vector2Int b)
{
	float xDis = a.x - b.x;
	float yDis = a.y - b.y;
	return sqrtf(xDis * xDis + yDis * yDis);
}

#pragma region Operators

Vector2Int operator+(const Vector2Int& left, const Vector2Int& right)
{
	return { left.x + right.x, left.y + right.y };
}

Vector2Int operator-(const Vector2Int& left, const Vector2Int& right)
{
	return { left.x - right.x, left.y - right.y };
}

Vector2Int operator*(const float value, const Vector2Int& vec)
{
	return { vec.x * value, vec.y * value };
}

Vector2Int operator*(const Vector2Int& left, const Vector2Int& right)
{
	return { left.x * right.x, left.y * right.y };
}

Vector2Int operator*(const Vector2Int& vec, const float value)
{
	return { vec.x * value, vec.y * value};
}

Vector2Int operator/(const float value, const Vector2Int& vec)
{
	return { vec.x / value, vec.y / value };
}

Vector2Int operator/(const Vector2Int& vec, const float value)
{
	return { vec.x / value, vec.y / value };
}

Vector2Int operator/(const Vector2Int& left, const Vector2Int& right)
{
	return { left.x / right.x, left.y / right.y };
}

Vector2Int operator/=(Vector2Int& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	return vec;
}

Vector2Int operator*=(Vector2Int& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	return vec;
}

Vector2Int operator+=(Vector2Int& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	return vec;
}

Vector2Int operator-=(Vector2Int& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
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