// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class RigidBody;
class BoxCollider;

/**
* @brief Class to manage collisions
*/
class PhysicsManager
{
public:
	/**
	* @brief Initialize the physics manager
	*/
	static void Init();

	/**
	* @brief Update the physics manager
	*/
	static void Update();

	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
	static std::vector<std::weak_ptr<BoxCollider>> boxColliders;
};

