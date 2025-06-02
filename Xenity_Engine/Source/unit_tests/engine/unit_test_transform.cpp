// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/math/vector2.h>
#include <engine/math/vector2_int.h>
#include <engine/math/vector3.h>
#include <engine/math/vector4.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/tools/gameplay_utility.h>

TestResult TransformSetPositionTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr<Transform> transform = gameObject->GetTransform();

	// Normal SetPosition
	transform->SetPosition(Vector3(1, 2, 3));
	EXPECT_EQUALS(transform->GetPosition(), Vector3(1, 2, 3), "Bad Transform SetPosition");

	// Normal SetLocalPosition
	transform->SetLocalPosition(Vector3(-1, -2, -3));
	EXPECT_EQUALS(transform->GetLocalPosition(), Vector3(-1, -2, -3), "Bad Transform SetLocalPosition");

	// SetPosition in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetPosition(Vector3(10, 20, 30));
	transform->SetPosition(Vector3(4, 5, 6));
	EXPECT_EQUALS(transform->GetPosition(), Vector3(4, 5, 6), "Bad Transform SetPosition in a parent (GetPosition)");
	EXPECT_EQUALS(transform->GetLocalPosition(), Vector3(-6, -15, -24), "Bad Transform SetPosition in a parent (GetLocalPosition)");

	// SetLocalPosition in a parent
	transform->SetLocalPosition(Vector3(4, 5, 6));
	EXPECT_EQUALS(transform->GetPosition(), Vector3(14, 25, 36), "Bad Transform SetLocalPosition in a parent (GetPosition)");
	EXPECT_EQUALS(transform->GetLocalPosition(), Vector3(4, 5, 6), "Bad Transform SetLocalPosition in a parent (GetLocalPosition)");

	Destroy(gameObject);
	Destroy(parent);

	END_TEST();
}

TestResult TransformSetRotationTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr <Transform> transform = gameObject->GetTransform();

	// Normal SetRotation
	transform->SetRotation(Vector3(90, 180, 270));
	EXPECT_EQUALS(transform->GetEulerAngles(), Vector3(90, 180, 270), "Bad Transform SetRotation");

	// Normal SetLocalRotation
	transform->SetLocalRotation(Vector3(-90, -180, -270));
	EXPECT_EQUALS(transform->GetLocalEulerAngles(), Vector3(-90, -180, -270), "Bad Transform SetLocalRotation");

	// SetRotation in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetRotation(Vector3(10, 20, 30));
	transform->SetRotation(Vector3(10, 20, 30));
	EXPECT_EQUALS(transform->GetEulerAngles(), Vector3(10, 20, 30), "Bad Transform SetRotation in a parent (GetEulerAngles)");
	EXPECT_EQUALS(transform->GetLocalEulerAngles(), Vector3(0, 0, 0), "Bad Transform SetRotation in a parent (GetLocalEulerAngles)");

	// SetLocalRotation in a parent
	transform->SetLocalRotation(Vector3(10, 20, 30));
	EXPECT_EQUALS(transform->GetEulerAngles(), Vector3(8.21814728f, 42.4855080f, 61.8378067f), "Bad Transform SetLocalRotation in a parent (GetEulerAngles)");
	EXPECT_EQUALS(transform->GetLocalEulerAngles(), Vector3(10, 20, 30), "Bad Transform SetLocalRotation in a parent (GetLocalEulerAngles)");

	Destroy(gameObject);
	Destroy(parent);

	END_TEST();
}

TestResult TransformSetScaleTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> gameObject = CreateGameObject();
	std::shared_ptr <Transform> transform = gameObject->GetTransform();

	// Normal SetRotation
	transform->SetLocalScale(Vector3(2, 3, 4));
	EXPECT_EQUALS(transform->GetScale(), Vector3(2, 3, 4), "Bad Transform SetLocalScale (GetScale)");
	EXPECT_EQUALS(transform->GetLocalScale(), Vector3(2, 3, 4), "Bad Transform SetLocalScale (GetLocalScale)");

	// SetRotation in a parent
	std::shared_ptr<GameObject> parent = CreateGameObject();
	gameObject->SetParent(parent);
	parent->GetTransform()->SetLocalScale(Vector3(2, 3, 4));
	transform->SetLocalScale(Vector3(2, 3, 4));
	EXPECT_EQUALS(transform->GetScale(), Vector3(4, 9, 16), "Bad Transform SetLocalScale in a parent (GetScale)");
	EXPECT_EQUALS(transform->GetLocalScale(), Vector3(2, 3, 4), "Bad Transform SetLocalScale in a parent (GetLocalScale)");

	Destroy(gameObject);
	Destroy(parent);

	END_TEST();
}