#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../reflection/reflection.h"

class API Vector2Int : public Reflection
{
public:
	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection();

	Vector2Int();
	Vector2Int(const int x, const int y);
	Vector2Int(const int fillValue);

	int x;
	int y;

	Vector2Int normalize();
	float magnitude();
	static float Distance(const Vector2Int& a, const Vector2Int& b);
};


API Vector2Int operator+(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator-(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator*(const Vector2Int& vec, const float value);
API Vector2Int operator*(const float value, const Vector2Int& vec);
API Vector2Int operator*(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator/(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator/(const Vector2Int& vec, const float value);
API Vector2Int operator/(const float value, const Vector2Int& vec);

API Vector2Int operator/=(Vector2Int& vec, const float value);
API Vector2Int operator*=(Vector2Int& vec, const float value);
API Vector2Int operator+=(Vector2Int& vec, const float value);
API Vector2Int operator-=(Vector2Int& vec, const float value);

API Vector2Int operator/=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator*=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator+=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator-=(Vector2Int& vec, const Vector2Int& vecRight);

API bool operator==(const Vector2Int& left, const Vector2Int& right);
API bool operator!=(const Vector2Int& left, const Vector2Int& right);