// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/component.h>
#include <engine/vectors/vector3.h>
#include <vector>

class BoxCollider;

class btRigidBody;
class btCollisionShape;
class btCompoundShape;

/*
* @brief Component to add physics to the GameObject
*/
class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();

	void Awake() override;

	inline const Vector3& GetVelocity() const { return velocity; }
	void SetVelocity(const Vector3& _velocity);

	inline float GetDrag() const { return drag; }
	void SetDrag(float _drag);

	inline float GetBounce() const { return bounce; }
	void SetBounce(float _bounce);

	inline float GetGravityMultiplier() const  { return gravityMultiplier; }
	void SetGravityMultiplier(float _gravityMultiplier);

	inline float IsStatic() const  { return isStatic; }
	void SetIsStatic(float _isStatic);


protected:
	friend class BoxCollider;
	friend class SphereCollider;

	void AddShape(btCollisionShape* shape, const Vector3& offset);
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	friend class PhysicsManager;

	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 1;
	float bounce = 0.5f;
	float mass = 1;
	float gravityMultiplier = 1.0f;
	bool isStatic = false;
	btRigidBody* bulletRigidbody = nullptr;
	btCompoundShape* bulletCompoundShape = nullptr;
	std::vector<btCollisionShape*> shapes;

	/**
	 * @brief [Internal]
	 */
	void Tick();
};
