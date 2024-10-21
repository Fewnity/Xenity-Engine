// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/vectors/vector3.h>
#include <engine/event_system/event_system.h>
#include "collider.h"

class RigidBody;
class btCollisionShape;
class btRigidBody;

/**
* @brief Component to add a cube-shaped collider to a GameObject
*/
class API SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	void SetSize(const float& size);
	inline const float& GetSize() const
	{
		return m_size;
	}

	void SetOffset(const Vector3& offset);
	inline const Vector3& GetOffset() const
	{
		return m_offset;
	}

protected:

	friend class RigidBody;
	friend class InspectorMenu;
	friend class MainBarMenu;

	void Awake() override;

	void Start() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;


	void OnDrawGizmosSelected() override;
	void CreateCollision(bool forceCreation) override;
	void OnTransformScaled() override;
	void OnTransformUpdated() override;

	/**
	* @brief Set the default size of the box collider based on the mesh renderer
	*/
	void SetDefaultSize() override;

	const std::weak_ptr<RigidBody>& GetAttachedRigidbody()
	{
		return m_attachedRigidbody;
	}

	float m_size = 1;
	Vector3 m_offset = Vector3(0);
};
