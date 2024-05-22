// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "physics_manager.h"

#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"

std::vector<std::weak_ptr<RigidBody>> PhysicsManager::rigidBodies;
std::vector<std::weak_ptr<BoxCollider>> PhysicsManager::boxColliders;

void PhysicsManager::Init()
{
}

void PhysicsManager::Update()
{
	size_t rigidbodyCount = rigidBodies.size();
	for (int i = 0; i < rigidbodyCount; i++)
	{
		if (!rigidBodies[i].lock())
		{
			rigidBodies.erase(rigidBodies.begin() + i);
			i--;
			rigidbodyCount--;
		}
	}

	size_t colliderCount = boxColliders.size();
	for (int i = 0; i < colliderCount; i++)
	{
		if (!boxColliders[i].lock())
		{
			boxColliders.erase(boxColliders.begin() + i);
			i--;
			colliderCount--;
		}
	}

	for (int i = 0; i < rigidbodyCount; i++)
	{
		std::shared_ptr<RigidBody> rb = rigidBodies[i].lock();
		rb->Tick();
	}
}
