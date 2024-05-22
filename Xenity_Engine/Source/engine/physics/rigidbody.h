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

	void Update() override;

	/**
	 * @brief [Internal]
	 */
	void Tick();

	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 1;
	float bounce = 0.5f;
	float gravityMultiplier = 1.0f;
	bool isStatic = false;
private:
};
