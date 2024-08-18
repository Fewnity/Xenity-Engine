// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "raycast.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"
#include "physics_manager.h"

bool Raycast::Check(const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit)
{
	RaycastHit nearestHit;

	btVector3 start = btVector3(startPosition.x, startPosition.y, startPosition.z);
	btVector3 end = btVector3(startPosition.x + direction.x * maxDistance, startPosition.y + direction.y * maxDistance, startPosition.z + direction.z * maxDistance);
	btCollisionWorld::ClosestRayResultCallback closestResults(start, end);
	//closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	PhysicsManager::physDynamicsWorld->rayTest(start, end, closestResults);
	if (closestResults.hasHit()) 
	{
		nearestHit.hitPosition = Vector3(closestResults.m_hitPointWorld.x(), closestResults.m_hitPointWorld.y(), closestResults.m_hitPointWorld.z());
		nearestHit.hitCollider = std::dynamic_pointer_cast<Collider>((reinterpret_cast<Collider*>(closestResults.m_collisionObject->getUserPointer()))->shared_from_this());
		return true;
	}
	return false;
}