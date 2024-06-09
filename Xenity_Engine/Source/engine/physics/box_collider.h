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

	void Awake() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;


	void OnDrawGizmosSelected() override;



	Event<std::shared_ptr<BoxCollider>> onTriggerEvent;

	bool isTrigger = false;

	const Vector3& GetMin() const
	{
		return min;
	}

	const Vector3& GetMax() const
	{
		return max;
	}

	void SetSize(const Vector3& size);
	const Vector3& GetSize();

	void SetOffset(const Vector3& offset);
	const Vector3& GetOffset();



private:

	friend class RigidBody;
	friend class InspectorMenu;
	friend class MainBarMenu;

	/**
	* @brief Set the default size of the box collider based on the mesh renderer
	*/
	void SetDefaultSize();

	/**
	* @brief Calculate the bounding box of the box collider
	*/
	void CalculateBoundingBox();

	std::weak_ptr<RigidBody> GetAttachedRigidbody()
	{
		return attachedRigidbody;
	}

	/**
	* @brief [Internal] Check if two box colliders are triggering each other
	* @param a The first box collider
	* @param b The second box collider
	*/
	static bool CheckTrigger(const std::shared_ptr<BoxCollider>& a, const std::shared_ptr <BoxCollider>& b);

	/**
	 * @brief [Internal] Check if two box colliders are colliding with each other
	 * @param a The first box collider
	 * @param b The second box collider
	 * @param aVelocity The velocity of the first box collider
	 */
	static CollisionSide CheckCollision(const std::shared_ptr <BoxCollider>& a, const std::shared_ptr <BoxCollider>& b, const Vector3& aVelocity);

	std::weak_ptr<RigidBody> attachedRigidbody;
	Vector3 size = Vector3(1);
	Vector3 offset = Vector3(0);
	Vector3 min;
	Vector3 max;
};
