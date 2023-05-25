#pragma once

class Vector2Int
{
public:
	Vector2Int();
	Vector2Int(const int x, const int y);
	Vector2Int(const int fillValue);

	int x;
	int y;

	Vector2Int normalize();
	float magnitude();
	static float Distance(const Vector2Int a, const Vector2Int b);
};


Vector2Int operator+(const Vector2Int& left, const Vector2Int& right);
Vector2Int operator-(const Vector2Int& left, const Vector2Int& right);
Vector2Int operator*(const Vector2Int& vec, const float value);
Vector2Int operator*(const float value, const Vector2Int& vec);
Vector2Int operator*(const Vector2Int& left, const Vector2Int& right);
Vector2Int operator/(const Vector2Int& left, const Vector2Int& right);
Vector2Int operator/(const Vector2Int& vec, const float value);
Vector2Int operator/(const float value, const Vector2Int& vec);

Vector2Int operator/=(Vector2Int& vec, const float value);
Vector2Int operator*=(Vector2Int& vec, const float value);
Vector2Int operator+=(Vector2Int& vec, const float value);
Vector2Int operator-=(Vector2Int& vec, const float value);

Vector2Int operator/=(Vector2Int& vec, const Vector2Int& vecRight);
Vector2Int operator*=(Vector2Int& vec, const Vector2Int& vecRight);
Vector2Int operator+=(Vector2Int& vec, const Vector2Int& vecRight);
Vector2Int operator-=(Vector2Int& vec, const Vector2Int& vecRight);

bool operator==(const Vector2Int& left, const Vector2Int& right);
bool operator!=(const Vector2Int& left, const Vector2Int& right);