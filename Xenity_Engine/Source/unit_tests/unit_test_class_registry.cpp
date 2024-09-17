// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/class_registry/class_registry.h>
#include <engine/lighting/lighting.h>

bool ClassRegistryTest::Start(std::string& errorOut)
{
	bool result = true;

	ClassRegistry::Reset();

	std::shared_ptr<GameObject> newGameObject = CreateGameObject();
	std::shared_ptr<Light> newLight = newGameObject->AddComponent<Light>();

	return result;
}