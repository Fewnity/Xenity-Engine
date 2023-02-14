#include "vector3.h"

Vector3::Vector3() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(float fillValue) {
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
}

Vector3 operator+(const Vector3& left, const Vector3& right)
{
	return { left.x + right.x, left.y + right.y, left.z + right.z };
}

Vector3 operator-(const Vector3& left, const Vector3& right)
{
	return { left.x - right.x, left.y - right.y, left.z - right.z };
}

Vector3 operator*(float value, const Vector3& vec)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}

Vector3 operator*(const Vector3& left, const Vector3& right)
{
	return { left.x * right.x, left.y * right.y, left.z * right.z };
}

Vector3 operator*(const Vector3& vec, float value)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}

Vector3 operator/(float value, const Vector3& vec)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}

Vector3 operator/(const Vector3& vec, float value)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}

Vector3 operator/(const Vector3& left, const Vector3& right)
{
	return { left.x / right.x, left.y / right.y, left.z / right.z };
}

Vector3 operator/=(Vector3& vec, float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	return vec;
}

Vector3 operator*=(Vector3& vec, float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	return vec;
}

Vector3 operator+=(Vector3& vec, float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	return vec;
}

Vector3 operator-=(Vector3& vec, float value)
{
	vec.x -= value;
	vec.y -= value;
	vec.z -= value;
	return vec;
}

Vector3 operator/=(Vector3& vec, const Vector3& vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	vec.z /= vecRight.z;
	return vec;
}

Vector3 operator*=(Vector3& vec, const Vector3& vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	vec.z *= vecRight.z;
	return vec;
}

Vector3 operator+=(Vector3& vec, const Vector3& vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	vec.z += vecRight.z;
	return vec;
}

Vector3 operator-=(Vector3& vec, const Vector3& vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	vec.z -= vecRight.z;
	return vec;
}

bool operator==(const Vector3& left, const Vector3& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z;
}

bool operator!=(const Vector3& left, const Vector3& right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z;
}