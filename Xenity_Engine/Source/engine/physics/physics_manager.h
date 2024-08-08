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
class btRigidBody;
class btVector3;
class btQuaternion;
class btDynamicsWorld;
class Collider;

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

	static void Stop();

	/**
	* @brief Update the physics manager
	*/
	static void Update();

	static void Clear();

	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
	static std::vector<std::weak_ptr<Collider>> colliders;
	static std::vector<btRigidBody*> mBodies;

	static btDynamicsWorld* physDynamicsWorld;
};

