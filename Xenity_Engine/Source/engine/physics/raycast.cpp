// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "raycast.h"

#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"
#include "physics_manager.h"
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>

bool Raycast::Check(const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit)
{
	RaycastHit nearestHit;
	nearestHit.hitGameObject.reset();
	nearestHit.hitCollider.reset();

	const size_t colliderCount = PhysicsManager::boxColliders.size();
	for (size_t i = 0; i < colliderCount; i++)
	{
		const std::shared_ptr<BoxCollider> boxCollider = PhysicsManager::boxColliders[i].lock();
		if (boxCollider && boxCollider->GetIsEnabled() && boxCollider->GetGameObject()->GetLocalActive())
		{
			RaycastHit currentHit;
			if (Check(boxCollider, startPosition, direction, maxDistance, currentHit))
			{
				if (nearestHit.hitGameObject.lock() == nullptr || currentHit.distance <= nearestHit.distance)
				{
					nearestHit = currentHit;
				}
			}
		}
	}

	raycastHit = nearestHit;
	return nearestHit.hitGameObject.lock() != nullptr;
}

bool Raycast::Check(const std::weak_ptr<BoxCollider> boxCollider, const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit)
{
	bool found = false;
	if (auto sharedboxCollider = boxCollider.lock())
	{
		const float epsilon = 1e-6f;
		Vector3 dirfrac;
		dirfrac.x = (fabs(direction.x) < epsilon) ? 1.0f : 1.0f / direction.x;
		dirfrac.y = (fabs(direction.y) < epsilon) ? 1.0f : 1.0f / direction.y;
		dirfrac.z = (fabs(direction.z) < epsilon) ? 1.0f : 1.0f / direction.z;

		const Vector3 min = sharedboxCollider->GetMin() + sharedboxCollider->GetTransform()->GetPosition();
		const Vector3 max = sharedboxCollider->GetMax() + sharedboxCollider->GetTransform()->GetPosition();

		const float t1 = (min.x - startPosition.x) * dirfrac.x;
		const float t2 = (max.x - startPosition.x) * dirfrac.x;
		const float t3 = (min.y - startPosition.y) * dirfrac.y;
		const float t4 = (max.y - startPosition.y) * dirfrac.y;
		const float t5 = (min.z - startPosition.z) * dirfrac.z;
		const float t6 = (max.z - startPosition.z) * dirfrac.z;

		const float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		const float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us, if tmin > tmax, ray doesn't intersect AABB
		if (tmax < 0 || tmin > tmax)
		{
		}
		else
		{
			if (tmin <= maxDistance)
			{
				found = true;
				raycastHit.distance = tmin;
				raycastHit.hitGameObject = sharedboxCollider->GetGameObject();
				raycastHit.hitCollider = sharedboxCollider;
				raycastHit.hitPosition = (direction * tmin) + startPosition;
			}
		}
	}

	return found;
}
