#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../reflection/reflection.h"

class Vector2Int;
class Vector2;

class API Vector3 : public Reflection
{
public:
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const float fillValue);
	Vector3(Vector2Int vect);
	Vector3(Vector2 vect);

	static Vector3 LookAt(const Vector3& from, const Vector3& to);
	static float Distance(const Vector3& a, const Vector3& b);
	static Vector3 Lerp(const Vector3& a, const Vector3& b, const int t);
	Vector3 Normalised();
	float Magnitude();
	float x;
	float y;
	float z;
};


API Vector3 operator+(const Vector3& left, const Vector3& right);
API Vector3 operator-(const Vector3& left, const Vector3& right);
API Vector3 operator-(const Vector3& vec);
API Vector3 operator*(const Vector3& vec, const float value);
API Vector3 operator*(const float value, const Vector3& vec);
API Vector3 operator*(const Vector3& left, const Vector3& right);
API Vector3 operator/(const Vector3& left, const Vector3& right);
API Vector3 operator/(const Vector3& vec, const float value);
API Vector3 operator/(const float value, const Vector3& vec);

API Vector3 operator/=(Vector3& vec, const float value);
API Vector3 operator*=(Vector3& vec, const float value);
API Vector3 operator+=(Vector3& vec, const float value);
API Vector3 operator-=(Vector3& vec, const float value);

API Vector3 operator/=(Vector3& vec, const Vector3& vecRight);
API Vector3 operator*=(Vector3& vec, const Vector3& vecRight);
API Vector3 operator+=(Vector3& vec, const Vector3& vecRight);
API Vector3 operator-=(Vector3& vec, const Vector3& vecRight);

API bool operator==(const Vector3& left, const Vector3& right);
API bool operator!=(const Vector3& left, const Vector3& right);