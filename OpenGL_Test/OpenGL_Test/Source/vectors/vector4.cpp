#include "vector4.h"

Vector4::Vector4() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 0;
}

Vector4::Vector4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(float fillValue) {
	this->x = fillValue;
	this->y = fillValue;
	this->z = fillValue;
	this->w = fillValue;
}

Vector4 operator*(float value, const Vector4& vec)
{
	return {vec.x * value, vec.y * value, vec.z * value, vec.w * value};
}
Vector4 operator*(const Vector4& vec, float value)
{
	return {vec.x * value, vec.y * value, vec.z * value, vec.w * value};
}