#include "vector4.h"

#pragma region Constructors / Destructor

Vector4::Vector4() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 0;
}

Vector4::Vector4(const float x, const float y, const float z, const float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const float fillValue) {
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
	this->w = fillValue;
}

#pragma endregion

#pragma region Operators

Vector4 operator+(const Vector4& left, const Vector4& right)
{
	return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
}

Vector4 operator-(const Vector4& left, const Vector4& right)
{
	return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}

Vector4 operator*(const float value, const Vector4& vec)
{
	return { vec.x * value, vec.y * value, vec.z * value, vec.w * value };
}

Vector4 operator*(const Vector4& left, const Vector4& right)
{
	return { left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w };
}

Vector4 operator*(const Vector4& vec, const float value)
{
	return { vec.x * value, vec.y * value, vec.z * value, vec.w * value };
}

Vector4 operator/(const float value, const Vector4& vec)
{
	return { vec.x / value, vec.y / value, vec.z / value, vec.w / value };
}

Vector4 operator/(const Vector4& vec, const float value)
{
	return { vec.x / value, vec.y / value, vec.z / value, vec.w / value };
}

Vector4 operator/(const Vector4& left, const Vector4& right)
{
	return { left.x / right.x, left.y / right.y, left.z / right.z, left.w / right.w };
}

Vector4 operator/=(Vector4& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	vec.w /= value;
	return vec;
}

Vector4 operator*=(Vector4& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	vec.w *= value;
	return vec;
}

Vector4 operator+=(Vector4& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	vec.w += value;
	return vec;
}

Vector4 operator-=(Vector4& vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	vec.z -= value;
	vec.w -= value;
	return vec;
}

Vector4 operator/=(Vector4& vec, const Vector4& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	vec.z /= vecRight.z;
	vec.w /= vecRight.w;
	return vec;
}

Vector4 operator*=(Vector4& vec, const Vector4& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	vec.z *= vecRight.z;
	vec.w *= vecRight.w;
	return vec;
}

Vector4 operator+=(Vector4& vec, const Vector4& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	vec.z += vecRight.z;
	vec.w += vecRight.w;
	return vec;
}

Vector4 operator-=(Vector4& vec, const Vector4& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	vec.z -= vecRight.z;
	vec.w -= vecRight.w;
	return vec;
}

bool operator==(const Vector4& left, const Vector4& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

bool operator!=(const Vector4& left, const Vector4& right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w;
}

#pragma endregion