#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../reflection/reflection.h"

class API Vector4 : public Reflection
{
public:
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	Vector4();
	Vector4(const float x, const float y, const float z, const float w);
	Vector4(const float fillValue);
	Vector4 Normalised();
	float Magnitude();
	static Vector4 Lerp(const Vector4& a, const Vector4& b, const float t);

	float x;
	float y;
	float z;
	float w;
};

API Vector4 operator+(const Vector4& left, const Vector4& right);
API Vector4 operator-(const Vector4& left, const Vector4& right);
API Vector4 operator*(const Vector4& vec, const float value);
API Vector4 operator*(const float value, const Vector4& vec);
API Vector4 operator*(const Vector4& left, const Vector4& right);
API Vector4 operator/(const Vector4& left, const Vector4& right);
API Vector4 operator/(const Vector4& vec, const float value);
API Vector4 operator/(const float value, const Vector4& vec);

API Vector4 operator/=(Vector4& vec, const float value);
API Vector4 operator*=(Vector4& vec, const float value);
API Vector4 operator+=(Vector4& vec, const float value);
API Vector4 operator-=(Vector4& vec, const float value);

API Vector4 operator/=(Vector4& vec, const Vector4& vecRight);
API Vector4 operator*=(Vector4& vec, const Vector4& vecRight);
API Vector4 operator+=(Vector4& vec, const Vector4& vecRight);
API Vector4 operator-=(Vector4& vec, const Vector4& vecRight);

API bool operator==(const Vector4& left, const Vector4& right);
API bool operator!=(const Vector4& left, const Vector4& right);