#include "vector4.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#pragma region Constructors / Destructor

/*void Vector4::SetReflection()
{
	reflectedVariables["x"] = x;
	reflectedVariables["y"] = y;
	reflectedVariables["z"] = z;
	reflectedVariables["w"] = w;
}*/

std::unordered_map<std::string, ReflectionEntry> Vector4::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, x, "x", true);
	Reflection::AddReflectionVariable(reflectedVariables, y, "y", true);
	Reflection::AddReflectionVariable(reflectedVariables, z, "z", true);
	Reflection::AddReflectionVariable(reflectedVariables, w, "w", true);
	return reflectedVariables;
}

Vector4::Vector4()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 0;
	//SetReflection();
}

Vector4::Vector4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	//SetReflection();
}

Vector4::Vector4(const float fillValue)
{
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
	this->w = fillValue;
	//SetReflection();
}

#pragma endregion

Vector4 Vector4::Normalise()
{
	float ls = this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
	float length;
	if (ls != 0)
	{
		length = sqrtf(ls);
		return Vector4(this->x / length, this->y / length, this->z / length, this->w / length);
	}
	else
	{
		return Vector4(0, 0, 0, 0);
	}
}

#pragma region Operators

Vector4 operator+(const Vector4 &left, const Vector4 &right)
{
	return {left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w};
}

Vector4 operator-(const Vector4 &left, const Vector4 &right)
{
	return {left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w};
}

Vector4 operator*(const float value, const Vector4 &vec)
{
	return {vec.x * value, vec.y * value, vec.z * value, vec.w * value};
}

Vector4 operator*(const Vector4 &left, const Vector4 &right)
{
	return {left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w};
}

Vector4 operator*(const Vector4 &vec, const float value)
{
	return {vec.x * value, vec.y * value, vec.z * value, vec.w * value};
}

Vector4 operator/(const float value, const Vector4 &vec)
{
	return {vec.x / value, vec.y / value, vec.z / value, vec.w / value};
}

Vector4 operator/(const Vector4 &vec, const float value)
{
	return {vec.x / value, vec.y / value, vec.z / value, vec.w / value};
}

Vector4 operator/(const Vector4 &left, const Vector4 &right)
{
	return {left.x / right.x, left.y / right.y, left.z / right.z, left.w / right.w};
}

Vector4 operator/=(Vector4 &vec, const float value)
{
	vec.x /= value;
	vec.y /= value;
	vec.z /= value;
	vec.w /= value;
	return vec;
}

Vector4 operator*=(Vector4 &vec, const float value)
{
	vec.x *= value;
	vec.y *= value;
	vec.z *= value;
	vec.w *= value;
	return vec;
}

Vector4 operator+=(Vector4 &vec, const float value)
{
	vec.x += value;
	vec.y += value;
	vec.z += value;
	vec.w += value;
	return vec;
}

Vector4 operator-=(Vector4 &vec, const float value)
{
	vec.x -= value;
	vec.y -= value;
	vec.z -= value;
	vec.w -= value;
	return vec;
}

Vector4 operator/=(Vector4 &vec, const Vector4 &vecRight)
{
	vec.x /= vecRight.x;
	vec.y /= vecRight.y;
	vec.z /= vecRight.z;
	vec.w /= vecRight.w;
	return vec;
}

Vector4 operator*=(Vector4 &vec, const Vector4 &vecRight)
{
	vec.x *= vecRight.x;
	vec.y *= vecRight.y;
	vec.z *= vecRight.z;
	vec.w *= vecRight.w;
	return vec;
}

Vector4 operator+=(Vector4 &vec, const Vector4 &vecRight)
{
	vec.x += vecRight.x;
	vec.y += vecRight.y;
	vec.z += vecRight.z;
	vec.w += vecRight.w;
	return vec;
}

Vector4 operator-=(Vector4 &vec, const Vector4 &vecRight)
{
	vec.x -= vecRight.x;
	vec.y -= vecRight.y;
	vec.z -= vecRight.z;
	vec.w -= vecRight.w;
	return vec;
}

bool operator==(const Vector4 &left, const Vector4 &right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

bool operator!=(const Vector4 &left, const Vector4 &right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w;
}

#pragma endregion