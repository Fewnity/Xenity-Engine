// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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

enum class CollisionSide
{
	NoSide = 0,
	SideX = 1,
	SideY = 2,
	SideZ = 4,
};

/**
* @brief Component to add a cube-shaped collider to a GameObject
*/
class API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();

	[[nodiscard]] inline const Vector3& GetMin() const
	{
		return m_min;
	}

	[[nodiscard]] inline const Vector3& GetMax() const
	{
		return m_max;
	}

	void SetSize(const Vector3& size);
	[[nodiscard]] inline const Vector3& GetSize() const
	{
		return s_size;
	}

	void SetOffset(const Vector3& offset);
	[[nodiscard]] inline const Vector3& GetOffset() const
	{
		return m_offset;
	}

	[[nodiscard]] std::string ToString() override;

protected:

	friend class RigidBody;
	friend class InspectorMenu;
	friend class MainBarMenu;

	void Awake() override;

	void Start() override;
	void CreateCollision(bool forceCreation) override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	void OnTransformScaled() override;
	void OnTransformUpdated() override;

	void OnDrawGizmosSelected() override;

	/**
	* @brief Set the default size of the box collider based on the mesh renderer
	*/
	void SetDefaultSize() override;

	/**
	* @brief Calculate the bounding box of the box collider
	*/
	void CalculateBoundingBox();

	Vector3 s_size = Vector3(1);
	Vector3 m_offset = Vector3(0);
	Vector3 m_min;
	Vector3 m_max;
};
