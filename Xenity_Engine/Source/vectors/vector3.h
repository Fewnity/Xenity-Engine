#pragma once

class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(float fillValue);
	float x;
	float y;
	float z;
};

Vector3 operator+(const Vector3& left, const Vector3& right);
Vector3 operator-(const Vector3& left, const Vector3& right);
Vector3 operator*(const Vector3& vec, float value);
Vector3 operator*(float value, const Vector3& vec);
Vector3 operator*(const Vector3& left, const Vector3& right);
Vector3 operator/(const Vector3& left, const Vector3& right);
Vector3 operator/(const Vector3& vec, float value);
Vector3 operator/(float value, const Vector3& vec);

Vector3 operator/=(Vector3& vec, float value);
Vector3 operator*=(Vector3& vec, float value);
Vector3 operator+=(Vector3& vec, float value);
Vector3 operator-=(Vector3& vec, float value);

Vector3 operator/=(Vector3& vec, const Vector3& vecRight);
Vector3 operator*=(Vector3& vec, const Vector3& vecRight);
Vector3 operator+=(Vector3& vec, const Vector3& vecRight);
Vector3 operator-=(Vector3& vec, const Vector3& vecRight);

bool operator==(const Vector3& left, const Vector3& right);
bool operator!=(const Vector3& left, const Vector3& right);