// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/math/vector3.h>
#include <engine/event_system/event_system.h>
#include "collider.h"

class RigidBody;
class btCollisionShape;
class btRigidBody;

/**
* @brief Component to add a sphere-shaped collider to a GameObject
*/
class API SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	/**
	* @brief Get the size of the sphere collider
	*/
	[[nodiscard]] float GetSize() const
	{
		return m_size;
	}

	/**
	* @brief Set the radius of the sphere collider
	*/
	void SetSize(float size);

	/**
	* @brief Get the offset position of the sphere collider
	*/
	[[nodiscard]] const Vector3& GetOffset() const
	{
		return m_offset;
	}

	/**
	* @brief Set the offset position of the sphere collider
	*/
	void SetOffset(const Vector3& offset);

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

	[[nodiscard]] const std::weak_ptr<RigidBody>& GetAttachedRigidbody()
	{
		return m_attachedRigidbody;
	}

	float m_size = 1;
	Vector3 m_offset = Vector3(0);
};
