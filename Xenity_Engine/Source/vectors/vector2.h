#pragma once

class Vector2
{
public:
	Vector2();
	Vector2(float x, float y);
	Vector2(float fillValue);
	float x;
	float y;
};

Vector2 operator*(const Vector2& vec, float value);
Vector2 operator*(float value, const Vector2& vec);