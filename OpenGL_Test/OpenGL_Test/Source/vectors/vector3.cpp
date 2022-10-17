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

Vector3 operator*(float value, const Vector3& vec)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}
Vector3 operator*(const Vector3& vec, float value)
{
	return { vec.x * value, vec.y * value, vec.z * value };
}

Vector3 operator/(float value, const Vector3& vec)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}
Vector3 operator/=(const Vector3& vec, float value)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}
Vector3 operator/(const Vector3& vec, float value)
{
	return { vec.x / value, vec.y / value, vec.z / value };
}