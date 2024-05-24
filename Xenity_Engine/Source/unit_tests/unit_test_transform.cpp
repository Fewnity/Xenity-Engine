// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/tools/gameplay_utility.h>

bool TransformSetPositionTest::Start(std::string& errorOut)
{
	bool result = true;

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr <Transform> transform = gameObject->GetTransform();

	// Normal SetPosition
	transform->SetPosition(Vector3(1, 2, 3));
	if (!Compare(transform->GetPosition(), Vector3(1, 2, 3)))
	{
		errorOut += "Bad Transform SetPosition\n";
		result = false;
	}

	// Normal SetLocalPosition
	transform->SetLocalPosition(Vector3(-1, -2, -3));
	if (!Compare(transform->GetPosition(), Vector3(-1, -2, -3)))
	{
		errorOut += "Bad Transform SetLocalPosition\n";
		result = false;
	}

	// SetPosition in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetPosition(Vector3(10, 20, 30));
	transform->SetPosition(Vector3(4, 5, 6));
	if (!Compare(transform->GetPosition(), Vector3(4, 5, 6)))
	{
		errorOut += "Bad Transform SetPosition in a parent (GetPosition)\n";
		result = false;
	}
	if (!Compare(transform->GetLocalPosition(), Vector3(-6, -15, -24)))
	{
		errorOut += "Bad Transform SetPosition in a parent (GetLocalPosition)\n";
		result = false;
	}

	// SetLocalPosition in a parent
	transform->SetLocalPosition(Vector3(4, 5, 6));
	if (!Compare(transform->GetPosition(), Vector3(14, 25, 36)))
	{
		errorOut += "Bad Transform SetLocalPosition in a parent (GetPosition)\n";
		result = false;
	}
	if (!Compare(transform->GetLocalPosition(), Vector3(4, 5, 6)))
	{
		errorOut += "Bad Transform SetLocalPosition in a parent (GetLocalPosition)\n";
		result = false;
	}

	Destroy(gameObject);
	Destroy(parent);

	return result;
}

bool TransformSetRotationTest::Start(std::string& errorOut)
{
	bool result = true;

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr <Transform> transform = gameObject->GetTransform();

	// Normal SetRotation
	transform->SetRotation(Vector3(90, 180, 270));
	if (!Compare(transform->GetRotation(), Vector3(90, 180, 270)))
	{
		errorOut += "Bad Transform SetRotation\n";
		result = false;
	}

	// Normal SetLocalRotation
	transform->SetLocalRotation(Vector3(-90, -180, -270));
	if (!Compare(transform->GetRotation(), Vector3(-90, -180, -270)))
	{
		errorOut += "Bad Transform SetLocalRotation\n";
		result = false;
	}

	// SetRotation in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetRotation(Vector3(10, 20, 30));
	transform->SetRotation(Vector3(10, 20, 30));
	if (!Compare(transform->GetRotation(), Vector3(10, 20, 30)))
	{
		errorOut += "Bad Transform SetRotation in a parent (GetRotation)\n";
		result = false;
	}
	if (!Compare(transform->GetLocalRotation(), Vector3(0, 0, 0)))
	{
		errorOut += "Bad Transform SetRotation in a parent (GetLocalRotation)\n";
		result = false;
	}

	// SetLocalRotation in a parent
	transform->SetLocalRotation(Vector3(10, 20, 30));
	if (!Compare(transform->GetRotation(), Vector3(8.21814728f, 42.4855042f, 61.8378143f)))
	{
		errorOut += "Bad Transform SetLocalRotation in a parent (GetRotation)\n";
		result = false;
	}
	if (!Compare(transform->GetLocalRotation(), Vector3(10, 20, 30)))
	{
		errorOut += "Bad Transform SetLocalRotation in a parent (GetLocalRotation)\n";
		result = false;
	}

	Destroy(gameObject);
	Destroy(parent);

	return result;
}

bool TransformSetScaleTest::Start(std::string& errorOut)
{
	bool result = true;

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr <Transform> transform = gameObject->GetTransform();

	// Normal SetRotation
	transform->SetLocalScale(Vector3(2, 3, 4));
	if (!Compare(transform->GetScale(), Vector3(2, 3, 4)))
	{
		errorOut += "Bad Transform SetLocalScale (GetScale)\n";
		result = false;
	}

	if (!Compare(transform->GetLocalScale(), Vector3(2, 3, 4)))
	{
		errorOut += "Bad Transform GetLocalScale (GetLocalScale)\n";
		result = false;
	}

	// SetRotation in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetLocalScale(Vector3(2, 3, 4));
	transform->SetLocalScale(Vector3(2, 3, 4));
	if (!Compare(transform->GetScale(), Vector3(4, 9, 16)))
	{
		errorOut += "Bad Transform SetLocalScale in a parent (GetScale)\n";
		result = false;
	}
	if (!Compare(transform->GetLocalScale(), Vector3(2, 3, 4)))
	{
		errorOut += "Bad Transform SetLocalScale in a parent (GetLocalScale)\n";
		result = false;
	}

	Destroy(gameObject);
	Destroy(parent);

	return result;
}