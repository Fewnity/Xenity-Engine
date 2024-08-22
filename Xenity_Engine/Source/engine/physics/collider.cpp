// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/physics/rigidbody.h>
#include <engine/game_elements/gameobject.h>
#include "physics_manager.h"

Collider::~Collider()
{
	if (std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		//Remove from the rigidbody
		const size_t colliderCount = rb->colliders.size();
		for (size_t i = 0; i < colliderCount; i++)
		{
			if (rb->colliders[i] == this)
			{
				rb->colliders.erase(rb->colliders.begin() + i);
				rb->UpdateGeneratesEvents();
				break;
			}
		}
	}

	if (m_bulletCollisionObject)
	{
		PhysicsManager::physDynamicsWorld->removeCollisionObject(m_bulletCollisionObject);

		delete m_bulletCollisionObject;
		m_bulletCollisionObject = nullptr;
	}

	if (m_bulletCollisionShape)
	{
		if (m_attachedRigidbody.lock())
		{
			if (!isTrigger)
				m_attachedRigidbody.lock()->RemoveShape(m_bulletCollisionShape);
			else
				m_attachedRigidbody.lock()->RemoveTriggerShape(m_bulletCollisionShape);
		}

		delete m_bulletCollisionShape;
		m_bulletCollisionShape = nullptr;
	}

}

void Collider::FindRigidbody()
{
	bool isAttached = m_attachedRigidbody.lock() != nullptr;
	m_attachedRigidbody = GetGameObject()->GetComponent<RigidBody>();
	if (std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->colliders.push_back(this);
			rb->UpdateGeneratesEvents();
		}
	}
}

void Collider::SetRigidbody(const std::shared_ptr<RigidBody>& rb)
{
	bool isAttached = m_attachedRigidbody.lock() != nullptr;
	m_attachedRigidbody = rb;
	if (std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->colliders.push_back(this);
			rb->UpdateGeneratesEvents();
		}
	}
	else 
	{
		CreateCollision(true);
	}
}

void Collider::OnEnabled()
{
	if (m_bulletCollisionObject)
		PhysicsManager::physDynamicsWorld->addCollisionObject(m_bulletCollisionObject);
}

void Collider::OnDisabled()
{
	if(m_bulletCollisionObject)
		PhysicsManager::physDynamicsWorld->removeCollisionObject(m_bulletCollisionObject);
}

void Collider::RemoveReferences()
{
	PhysicsManager::RemoveCollider(this);
}
