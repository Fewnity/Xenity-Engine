#pragma once
#include <engine/api.h>

#include <engine/reflection/reflection.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

class Vector2Int;
class Vector2;

/*
* @brief Contains 3D coordinates
*/
class API Vector3 : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const glm::vec3& glmVec3);
	Vector3(const glm::vec4& glmVec4);
	Vector3(const float fillValue);
	Vector3(const Vector2Int& vect);
	Vector3(const Vector2& vect);

	/**
	* @brief Get the look rotation in degrees between two vectors
	*/
	static Vector3 LookAt(const Vector3& from, const Vector3& to);

	/**
	* @brief Distance between two vectors
	*/
	static float Distance(const Vector3& a, const Vector3& b);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static Vector3 Lerp(const Vector3& a, const Vector3& b, const float t);

	/**
	* @brief Get this vector with a magnitude of 1 (Do not change vector values)
	*/
	Vector3 Normalized() const;

	/**
	* @brief Makes this vector have a magnitude of 1 (Change vector values)
	*/
	Vector3 Normalize();

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	/**
	* @brief Get the dot product of this vector
	*/
	double Dot(const Vector3& v) const;

	/**
	* @brief Get the cross product of this vector and another
	*/
	Vector3 Cross(const Vector3& v) const;

	/**
	* @brief Get the dot product of two vectors
	*/
	static double Dot(const Vector3& a, const Vector3& b);

	/**
	* @brief Get the cross product of two vectors
	*/
	static Vector3 Cross(const Vector3& a, const Vector3& b);

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