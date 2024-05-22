#pragma once
#include <engine/api.h>

class Vector3;
class Vector2Int;

#include <engine/reflection/reflection.h>

/*
* @brief Contains 2D coordinates
*/
class API Vector2 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector2();
	Vector2(const float x, const float y);
	Vector2(const float fillValue);
	Vector2(const Vector3& vect3);
	Vector2(const Vector2Int& vect2Int);

	/**
	* @brief Distance between two vectors
	*/
	static float Distance(const Vector2& a, const Vector2& b);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static Vector2 Lerp(const Vector2& a, const Vector2& b, const float t);

	/**
	* @brief Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector2 Normalized() const;

	/**
	* @brief Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector2 Normalize();

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	float x;
	float y;
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