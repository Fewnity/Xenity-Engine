#pragma once
#include <engine/api.h>

class Vector4;
class Vector3;
class Vector2Int;
class Vector2;

#include <glm/glm.hpp>

class API Math
{
public:

	static const float PI;

	/**
	* Multiply two matrices
	* @param A First matrix
	* @param B Second matrix
	* @param result Result matrix (should be already allocated)
	* @param rA Row Count of A
	* @param cA Column Count of A
	* @param rB Row Count of B
	* @param cB Column Count of B
	*/
	static void MultiplyMatrices(const float* A, const float* B, float* result, int rA, int cA, int rB, int cB);

	static glm::mat4 CreateModelMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale);

	/**
	* Get the next power of 2 of the given value (if the value is not itself a power of two)
	* Ex Value = 140; returns -> 256
	* Ex Value = 128; returns -> 128
	* @param value Start value
	*/
	static unsigned int nextPow2(const unsigned int value);

	/**
	* Get the previous power of 2 of the given value (if the value is not itself a power of two)
	* Ex Value = 140; returns -> 128
	* Ex Value = 128; returns -> 128
	* @param value Start value
	*/
	static unsigned int previousPow2(const unsigned int value);

	/**
	* Get a normalised 3D direction from two angles
	* @param angleA
	* @param angleB
	*/
	static Vector3 Get3DDirectionFromAngles(const float angleA, const float angleB);

	/**
	* Get a normalised 2D direction from an angle
	* @param angle
	*/
	static Vector2 Get2DDirectionFromAngle(const float angle);

	/**
	* Linearly interpolates between a and b by t
	* @param a Start value
	* @param b End value
	* @param t [0,1]
	*/
	static float Lerp(float a, float b, float t);

	/**
	* Restrict a number between two other numbers
	* @param value Value to clamp
	* @param min Minimum
	* @param mac Maximum
	*/
	static float Clamp(float value, float min, float max);
};