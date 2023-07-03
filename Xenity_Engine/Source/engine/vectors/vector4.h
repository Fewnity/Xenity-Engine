#pragma once

class Vector4
{
public:
	Vector4();
	Vector4(const float x, const float y, const float z, const float w);
	Vector4(const float fillValue);
	Vector4 Normalise();
	float x;
	float y;
	float z;
	float w;
};

Vector4 operator+(const Vector4& left, const Vector4& right);
Vector4 operator-(const Vector4& left, const Vector4& right);
Vector4 operator*(const Vector4& vec, const float value);
Vector4 operator*(const float value, const Vector4& vec);
Vector4 operator*(const Vector4& left, const Vector4& right);
Vector4 operator/(const Vector4& left, const Vector4& right);
Vector4 operator/(const Vector4& vec, const float value);
Vector4 operator/(const float value, const Vector4& vec);

Vector4 operator/=(Vector4& vec, const float value);
Vector4 operator*=(Vector4& vec, const float value);
Vector4 operator+=(Vector4& vec, const float value);
Vector4 operator-=(Vector4& vec, const float value);

Vector4 operator/=(Vector4& vec, const Vector4& vecRight);
Vector4 operator*=(Vector4& vec, const Vector4& vecRight);
Vector4 operator+=(Vector4& vec, const Vector4& vecRight);
Vector4 operator-=(Vector4& vec, const Vector4& vecRight);

bool operator==(const Vector4& left, const Vector4& right);
bool operator!=(const Vector4& left, const Vector4& right);