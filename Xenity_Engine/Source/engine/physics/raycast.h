// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/api.h>
#include <engine/math/vector3.h>

class RigidBody;
class BoxCollider;
class Collider;

struct RaycastHit 
{
	std::weak_ptr<GameObject> hitGameObject;
	std::weak_ptr<Collider> hitCollider;
	Vector3 hitPosition = Vector3(0, 0, 0);
	float distance = 0;
};

/**
* @brief Class to check collisions with a ray
*/
class API Raycast
{
public:
	/**
	* @brief Check if a ray hits a collider
	* @param startPosition The start position of the ray
	* @param direction The direction of the ray
	* @param maxDistance The max distance of the ray
	* @param raycastHit The raycastHit struct that will be filled with the hit information
	*/
	[[nodiscard]] static bool Check(const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit);
private:
};

