#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <engine/reflection/reflection.h>

class Vector2Int;
class Vector2;

class API Vector3 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const float fillValue);
	Vector3(Vector2Int vect);
	Vector3(Vector2 vect);

	/**
	* Get the look rotation in degrees between two vectors
	*/
	static Vector3 LookAt(const Vector3& from, const Vector3& to);

	/**
	* Distance between two vectors
	*/
	static float Distance(const Vector3& a, const Vector3& b);

	/**
	* Linearly interpolates between vectors
	*/
	static Vector3 Lerp(const Vector3& a, const Vector3& b, const float t);

	/**
	* Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector3 Normalized() const;

	/**
	* Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector3 Normalize();

	/**
	* Get the length of this vector
	*/
	float Magnitude() const;

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