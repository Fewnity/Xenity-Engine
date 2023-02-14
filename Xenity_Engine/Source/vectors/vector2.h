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


Vector2 operator+(const Vector2& left, const Vector2& right);
Vector2 operator-(const Vector2& left, const Vector2& right);
Vector2 operator*(const Vector2& vec, float value);
Vector2 operator*(float value, const Vector2& vec);
Vector2 operator*(const Vector2& left, const Vector2& right);
Vector2 operator/(const Vector2& left, const Vector2& right);
Vector2 operator/(const Vector2& vec, float value);
Vector2 operator/(float value, const Vector2& vec);

Vector2 operator/=(Vector2& vec, float value);
Vector2 operator*=(Vector2& vec, float value);
Vector2 operator+=(Vector2& vec, float value);
Vector2 operator-=(Vector2& vec, float value);

Vector2 operator/=(Vector2& vec, const Vector2& vecRight);
Vector2 operator*=(Vector2& vec, const Vector2& vecRight);
Vector2 operator+=(Vector2& vec, const Vector2& vecRight);
Vector2 operator-=(Vector2& vec, const Vector2& vecRight);

bool operator==(const Vector2& left, const Vector2& right);
bool operator!=(const Vector2& left, const Vector2& right);