// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/component.h>
#include <engine/vectors/vector3.h>

class BoxCollider;

/*
* @brief Component to add physics to the GameObject
*/
class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	inline Vector3 GetVelocity() { return velocity; }
	void SetVelocity(Vector3 _velocity);

	inline float GetDrag() { return drag; }
	void SetDrag(float _drag);

	inline float GetBounce() { return bounce; }
	void SetBounce(float _bounce);

	inline float GetGravityMultiplier() { return gravityMultiplier; }
	void SetGravityMultiplier(float _gravityMultiplier);

	inline float IsStatic() { return isStatic; }
	void SetIsStatic(float _isStatic);

private:
	friend class PhysicsManager;

	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 1;
	float bounce = 0.5f;
	float gravityMultiplier = 1.0f;
	bool isStatic = false;

	/**
	 * @brief [Internal]
	 */
	void Tick();
};
