// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include "collider.h"
#include <engine/vectors/vector3.h>
#include <engine/event_system/event_system.h>
#include <memory>

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

	Event<std::shared_ptr<SphereCollider>> onTriggerEvent;

	bool isTrigger = false;

	inline const Vector3& GetMin() const
	{
		return min;
	}

	inline const Vector3& GetMax() const
	{
		return max;
	}

	void SetSize(const float& size);
	inline const float& GetSize() const
	{
		return size;
	}

	void SetOffset(const Vector3& offset);
	inline const Vector3& GetOffset() const
	{
		return offset;
	}

protected:

	btRigidBody* bulletRigidbody = nullptr;
	btCollisionShape* bulletCollisionShape = nullptr;

	friend class RigidBody;
	friend class InspectorMenu;
	friend class MainBarMenu;

	void Awake() override;

	void Start() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;


	void OnDrawGizmosSelected() override;

	/**
	* @brief Set the default size of the box collider based on the mesh renderer
	*/
	void SetDefaultSize();

	/**
	* @brief Calculate the bounding box of the box collider
	*/
	void CalculateBoundingBox();

	const std::weak_ptr<RigidBody>& GetAttachedRigidbody()
	{
		return attachedRigidbody;
	}

	/**
	* @brief [Internal] Check if two box colliders are triggering each other
	* @param a The first box collider
	* @param b The second box collider
	*/
	static bool CheckTrigger(const SphereCollider& a, const SphereCollider& b);

	std::weak_ptr<RigidBody> attachedRigidbody;
	float size = 1;
	Vector3 offset = Vector3(0);
	Vector3 min;
	Vector3 max;
};
