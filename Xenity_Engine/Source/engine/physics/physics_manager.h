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
#include <unordered_map>

class RigidBody;
class BoxCollider;
class btRigidBody;
class btVector3;
class btQuaternion;
class btDynamicsWorld;
class Collider;

enum class CollisionState 
{
	FirstFrame,
	RequireUpdate,
	Updated,
};

struct ColliderInfo
{
	struct CollisionInfo
	{
		Collider* otherCollider;
		CollisionState state = CollisionState::FirstFrame;
	};

	Collider* collider;
	std::unordered_map<Collider*, CollisionState> collisions;
	std::unordered_map<Collider*, CollisionState> triggersCollisions;
};

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

	static void AddRigidBody(RigidBody* rb);
	static void RemoveRigidBody(const RigidBody* rb);

	static void AddCollider(Collider* rb);
	static void RemoveCollider(const Collider* rb);
	static void AddEvent(Collider* collider, Collider* otherCollider, bool isTrigger);

	static btDynamicsWorld* physDynamicsWorld;
private:
	static void CallCollisionEvent(Collider* a, Collider* b, bool isTrigger, int state);

	static std::vector<RigidBody*> rigidBodies;
	static std::vector<ColliderInfo> colliders;

};