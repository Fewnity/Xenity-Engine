#pragma once

class Vector4
{
public:
	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(float fillValue);
	float x;
	float y;
	float z;
	float w;
};

Vector4 operator*(const Vector4& vec, float value);
Vector4 operator*(float value, const Vector4& vec);