// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

class Vector4;
class Vector3;
class Vector2Int;
class Vector2;
class Quaternion;

#include <glm/fwd.hpp>


/**
* @brief InternalMath class for basics operations
*/
class API InternalMath
{
public:

	/**
	* @brief Multiply two matrices
	* @param A First matrix
	* @param B Second matrix
	* @param result Result matrix (should be already allocated)
	* @param rA Row Count of A
	* @param cA Column Count of A
	* @param rB Row Count of B
	* @param cB Column Count of B
	*/
	static void MultiplyMatrices(const float* A, const float* B, float* result, int rA, int cA, int rB, int cB);

	/**
	* @brief Create a model matrix
	* @param position Position of the object
	* @param rotation Rotation of the object
	* @param scale Scale of the object
	* @return Model matrix
	*/
	[[nodiscard]] static glm::mat4 CreateModelMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale);
	[[nodiscard]] static glm::mat4 CreateModelMatrix(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

	[[nodiscard]] static glm::mat4 MultiplyMatrices(const glm::mat4& matA, const glm::mat4& matB);

	/**
	* @brief Get the next power of 2 of the given value (if the value is not itself a power of two)
	* @brief Ex Value = 140; returns -> 256
	* @brief Ex Value = 128; returns -> 128
	* @param value Start value
	*/
	[[nodiscard]] static unsigned int NextPow2(const unsigned int value);

	/**
	* @brief Get the previous power of 2 of the given value (if the value is not itself a power of two)
	* @brief Ex Value = 140; returns -> 128
	* @brief Ex Value = 128; returns -> 128
	* @param value Start value
	*/
	[[nodiscard]] static unsigned int PreviousPow2(const unsigned int value);

	/**
	* @brief Get a normalised 3D direction from two angles
	* @param angleA
	* @param angleB
	*/
	[[nodiscard]] static Vector3 Get3DDirectionFromAngles(const float angleA, const float angleB);

	/**
	* @brief Get a normalised 2D direction from an angle
	* @param angle
	*/
	[[nodiscard]] static Vector2 Get2DDirectionFromAngle(const float angle);
};