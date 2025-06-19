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
* @brief Component to add a cube-shaped collider to a GameObject
*/
class API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();

	/**
	* @brief Set the size of the box collider
	*/
	void SetSize(const Vector3& size);

	/**
	* @brief Set the offset position of the box collider
	*/
	void SetOffset(const Vector3& offset);

	/**
	* @brief Get the size of the box collider
	*/
	[[nodiscard]] const Vector3& GetSize() const
	{
		return m_size;
	}

	/**
	* @brief Get the offset position of the box collider
	*/
	[[nodiscard]] const Vector3& GetOffset() const
	{
		return m_offset;
	}

	/**
	* @brief Get the size and offset of the box collider in the form of a string
	*/
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

	Vector3 m_size = Vector3(1);
	Vector3 m_offset = Vector3(0);
	Vector3 m_min;
	Vector3 m_max;
};
