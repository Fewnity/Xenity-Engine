#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__) || defined(_EE)
#undef __STRICT_ANSI__
#endif

#include <cmath>
#include "vector3.h"
#include "vector2_int.h"
#include "vector2.h"

#pragma region Constructors / Destructor

ReflectiveData Vector3::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	return reflectedVariables;
}

Vector3::Vector3()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(const float fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
}

Vector3::Vector3(Vector2Int vect)
{
	this->x = (float)vect.x;
	this->y = (float)vect.y;
	this->z = 0;
}

Vector3::Vector3(Vector2 vect)
{
	this->x = vect.x;
	this->y = vect.y;
	this->z = 0;
}

#pragma endregion

Vector3 Vector3::LookAt(const Vector3& from, const Vector3& to)
{
	float xdis = to.x - from.x;
	float ydis = to.y - from.y;
	float zdis = to.z - from.z;
	float xzdis = sqrtf(xdis * xdis + zdis * zdis);

	return Vector3((-atan2f(ydis, xzdis)) * 180 / (float)M_PI, (-(atan2f(-xdis, zdis))) * 180 / (float)M_PI, 0);
}

Vector3 Vector3::Normalised()
{
	float ls = this->x * this->x + this->y * this->y + this->z * this->z;
	if (ls != 0)
	{
		float length = sqrtf(ls);
		return Vector3(this->x / length, this->y / length, this->z / length);
	}
	else
	{
		return Vector3(0, 0, 0);
	}
}

float Vector3::Magnitude()
{
	return sqrtf(powf(this->x, 2) + powf(this->y, 2) + powf(this->z, 2));
}

float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	float xDis = a.x - b.x;
	float yDis = a.y - b.y;
	float zDis = a.z - b.z;
	return sqrtf(xDis * xDis + yDis * yDis + zDis * zDis);
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, const float t)
{
	return a + (b-a) * t;
}

#pragma region Operators

Vector3 operator+(const Vector3& left, const Vector3& right)
{
	return { left.x + right.x, left.y + right.y, left.z + right.z };
}

Vector3 operator-(const Vector3& left, const Vector3& right)
{
	return { left.x - right.x, left.y - right.y, left.z - right.z };
}

Vector3 operator-(const Vector3& vec)
{
	return { -vec.x, -vec.y, -vec.z };
}

Vector3 operator*(float value, const Vector3& vec)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}

Vector3 operator*(const Vector3& left, const Vector3& right)
{
	return { left.x * right.x, left.y * right.y, left.z * right.z };
}

Vector3 operator*(const Vector3& vec, const float value)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}

Vector3 operator/(float value, const Vector3& vec)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}

Vector3 operator/(const Vector3& vec, const float value)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}

Vector3 operator/(const Vector3& left, const Vector3& right)
{
	return { left.x / right.x, left.y / right.y, left.z / right.z };
}

Vector3 operator/=(Vector3& vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	return vec;
}

Vector3 operator*=(Vector3& vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	return vec;
}

Vector3 operator+=(Vector3& vec, const float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	return vec;
}

Vector3 operator-=(Vector3& vec, const float value)
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

#pragma endregion