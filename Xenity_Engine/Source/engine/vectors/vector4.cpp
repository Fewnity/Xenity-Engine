#include "vector4.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__) || defined(_EE)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#pragma region Constructors / Destructor

ReflectiveData Vector4::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, x, "x", true);
	Reflective::AddVariable(reflectedVariables, y, "y", true);
	Reflective::AddVariable(reflectedVariables, z, "z", true);
	Reflective::AddVariable(reflectedVariables, w, "w", true);
	return reflectedVariables;
}

Vector4::Vector4()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 0;
}

Vector4::Vector4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const float fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
	this->w = fillValue;
}

#pragma endregion

Vector4 Vector4::Normalized() const
{
	float ls = this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
	if (ls != 0)
	{
		float length = sqrtf(ls);
		return Vector4(this->x / length, this->y / length, this->z / length, this->w / length);
	}
	else
	{
		return Vector4(0, 0, 0, 0);
	}
}

Vector4 Vector4::Normalize()
{
	*(this) = this->Normalized();
	return *(this);
}

float Vector4::Magnitude() const
{
	return sqrtf(powf(this->x, 2) + powf(this->y, 2) + powf(this->z, 2) + powf(this->w, 2));
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, const float t)
{
	return a + (b - a) * t;
}

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