#include "vector2.h"

Vector2::Vector2() {
	this->x = 0;
	this->y = 0;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2::Vector2(float fillValue) {
	this->x = fillValue;
	this->y = fillValue;
}

Vector2 operator*(float value, const Vector2& vec)
{
	return { vec.x * value, vec.y * value };
}
Vector2 operator*(const Vector2& vec, float value)
{
	return { vec.x * value, vec.y * value };
}