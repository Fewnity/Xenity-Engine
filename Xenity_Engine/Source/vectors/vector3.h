#pragma once

class Vector3
{
public:
	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const float fillValue);

	float x;
	float y;
	float z;

	static Vector3 LookAt(Vector3 from, Vector3 to);
	static float Distance(Vector3 a, Vector3 b);
	Vector3 Normalise();

};


Vector3 operator+(const Vector3& left, const Vector3& right);
Vector3 operator-(const Vector3& left, const Vector3& right);
Vector3 operator-(const Vector3& vec);
Vector3 operator*(const Vector3& vec, const float value);
Vector3 operator*(const float value, const Vector3& vec);
Vector3 operator*(const Vector3& left, const Vector3& right);
Vector3 operator/(const Vector3& left, const Vector3& right);
Vector3 operator/(const Vector3& vec, const float value);
Vector3 operator/(const float value, const Vector3& vec);

Vector3 operator/=(Vector3& vec, const float value);
Vector3 operator*=(Vector3& vec, const float value);
Vector3 operator+=(Vector3& vec, const float value);
Vector3 operator-=(Vector3& vec, const float value);

Vector3 operator/=(Vector3& vec, const Vector3& vecRight);
Vector3 operator*=(Vector3& vec, const Vector3& vecRight);
Vector3 operator+=(Vector3& vec, const Vector3& vecRight);
Vector3 operator-=(Vector3& vec, const Vector3& vecRight);

bool operator==(const Vector3& left, const Vector3& right);
bool operator!=(const Vector3& left, const Vector3& right);