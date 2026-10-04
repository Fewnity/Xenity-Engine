// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "raycast.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"
#include "physics_manager.h"

namespace
{
	// Closest hit callback which also keeps the child shape index of the hit (rigidbodies use compound shapes)
	struct ClosestRayResultWithChildCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		using btCollisionWorld::ClosestRayResultCallback::ClosestRayResultCallback;

		btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override
		{
			m_childIndex = rayResult.m_localShapeInfo ? rayResult.m_localShapeInfo->m_triangleIndex : -1;
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}

		int m_childIndex = -1;
	};
}

bool Raycast::Check(const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit)
{
	RaycastHit nearestHit;

	const btVector3 start = btVector3(startPosition.x, startPosition.y, startPosition.z);
	// Normalize the direction, otherwise the ray length would not be maxDistance
	const Vector3 normalizedDirection = direction.Normalized();
	const btVector3 end = btVector3(startPosition.x + normalizedDirection.x * maxDistance, startPosition.y + normalizedDirection.y * maxDistance, startPosition.z + normalizedDirection.z * maxDistance);
	ClosestRayResultWithChildCallback closestResults(start, end);
	//closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	PhysicsManager::s_physDynamicsWorld->rayTest(start, end, closestResults);
	if (closestResults.hasHit()) 
	{
		// The user pointer of a bullet rigidbody is a RigidBody, not a Collider
		Collider* hitCollider = PhysicsManager::GetColliderFromCollisionObject(closestResults.m_collisionObject, closestResults.m_childIndex);
		if (!hitCollider)
		{
			return false;
		}

		nearestHit.hitPosition = Vector3(closestResults.m_hitPointWorld.x(), closestResults.m_hitPointWorld.y(), closestResults.m_hitPointWorld.z());
		nearestHit.hitCollider = std::dynamic_pointer_cast<Collider>(hitCollider->shared_from_this());
		raycastHit = nearestHit;
		return true;
	}
	return false;
}