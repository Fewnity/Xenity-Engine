#pragma once

class Vector3;
class Vector2Int;

class Vector2
{
public:
	Vector2();
	Vector2(const float x, const float y);
	Vector2(const float fillValue);
	Vector2(const Vector3 vect3);
	Vector2(const Vector2Int vect2Int);

	float x;
	float y;

	Vector2 Normalize();
	float magnitude();
	static float Distance(const Vector2 a, const Vector2 b);
};


Vector2 operator+(const Vector2& left, const Vector2& right);
Vector2 operator-(const Vector2& left, const Vector2& right);
Vector2 operator*(const Vector2& vec, const float value);
Vector2 operator*(const float value, const Vector2& vec);
Vector2 operator*(const Vector2& left, const Vector2& right);
Vector2 operator/(const Vector2& left, const Vector2& right);
Vector2 operator/(const Vector2& vec, const float value);
Vector2 operator/(const float value, const Vector2& vec);

Vector2 operator/=(Vector2& vec, const float value);
Vector2 operator*=(Vector2& vec, const float value);
Vector2 operator+=(Vector2& vec, const float value);
Vector2 operator-=(Vector2& vec, const float value);

Vector2 operator/=(Vector2& vec, const Vector2& vecRight);
Vector2 operator*=(Vector2& vec, const Vector2& vecRight);
Vector2 operator+=(Vector2& vec, const Vector2& vecRight);
Vector2 operator-=(Vector2& vec, const Vector2& vecRight);

bool operator==(const Vector2& left, const Vector2& right);
bool operator!=(const Vector2& left, const Vector2& right);