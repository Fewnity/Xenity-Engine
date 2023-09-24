#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

class Vector3;
class Vector2Int;

#include "../reflection/reflection.h"

class API Vector2: public Reflection
{
public:
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	Vector2();
	Vector2(const float x, const float y);
	Vector2(const float fillValue);
	Vector2(const Vector3& vect3);
	Vector2(const Vector2Int& vect2Int);

	float x;
	float y;

	Vector2 Normalize();
	float magnitude();
	static float Distance(const Vector2& a, const Vector2& b);
};


API Vector2 operator+(const Vector2& left, const Vector2& right);
API Vector2 operator-(const Vector2& left, const Vector2& right);
API Vector2 operator*(const Vector2& vec, const float value);
API Vector2 operator*(const float value, const Vector2& vec);
API Vector2 operator*(const Vector2& left, const Vector2& right);
API Vector2 operator/(const Vector2& left, const Vector2& right);
API Vector2 operator/(const Vector2& vec, const float value);
API Vector2 operator/(const float value, const Vector2& vec);

API Vector2 operator/=(Vector2& vec, const float value);
API Vector2 operator*=(Vector2& vec, const float value);
API Vector2 operator+=(Vector2& vec, const float value);
API Vector2 operator-=(Vector2& vec, const float value);

API Vector2 operator/=(Vector2& vec, const Vector2& vecRight);
API Vector2 operator*=(Vector2& vec, const Vector2& vecRight);
API Vector2 operator+=(Vector2& vec, const Vector2& vecRight);
API Vector2 operator-=(Vector2& vec, const Vector2& vecRight);

API bool operator==(const Vector2& left, const Vector2& right);
API bool operator!=(const Vector2& left, const Vector2& right);