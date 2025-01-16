// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/physics/rigidbody.h>
#include <engine/game_elements/gameobject.h>
#include "physics_manager.h"

Collider::~Collider()
{
	if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		//Remove from the rigidbody
		const size_t colliderCount = rb->m_colliders.size();
		for (size_t i = 0; i < colliderCount; i++)
		{
			if (rb->m_colliders[i] == this)
			{
				rb->m_colliders.erase(rb->m_colliders.begin() + i);
				rb->UpdateGeneratesEvents();
				break;
			}
		}
	}

	if (m_bulletCollisionObject)
	{
		PhysicsManager::s_physDynamicsWorld->removeCollisionObject(m_bulletCollisionObject);

		delete m_bulletCollisionObject;
		m_bulletCollisionObject = nullptr;
	}

	if (m_bulletCollisionShape)
	{
		if (m_attachedRigidbody.lock())
		{
			if (!m_isTrigger)
			{
				m_attachedRigidbody.lock()->RemoveShape(m_bulletCollisionShape);
			}
			else
			{
				m_attachedRigidbody.lock()->RemoveTriggerShape(m_bulletCollisionShape);
			}
		}

		delete m_bulletCollisionShape;
		m_bulletCollisionShape = nullptr;
	}

}

void Collider::FindRigidbody()
{
	const bool isAttached = m_attachedRigidbody.lock() != nullptr;
	m_attachedRigidbody = GetGameObject()->GetComponent<RigidBody>();
	if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->m_colliders.push_back(this);
			rb->UpdateGeneratesEvents();
		}
	}
}

void Collider::SetRigidbody(const std::shared_ptr<RigidBody>& rb)
{
	const bool isAttached = m_attachedRigidbody.lock() != nullptr;
	m_attachedRigidbody = rb;
	if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->m_colliders.push_back(this);
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
	{
		PhysicsManager::s_physDynamicsWorld->addCollisionObject(m_bulletCollisionObject);
	}
}

void Collider::OnDisabled()
{
	if(m_bulletCollisionObject)
	{
		PhysicsManager::s_physDynamicsWorld->removeCollisionObject(m_bulletCollisionObject);
	}
}

void Collider::RemoveReferences()
{
	PhysicsManager::RemoveCollider(this);
}
