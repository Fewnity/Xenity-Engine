#include "vector2.h"

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