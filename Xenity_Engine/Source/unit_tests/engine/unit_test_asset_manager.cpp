// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/lighting/lighting.h>
#include <engine/game_elements/gameobject.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/game_elements/gameplay_manager.h>

TestResult AssetManagerTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> newGameObject = CreateGameObject();
	std::shared_ptr<Light> newLight = newGameObject->AddComponent<Light>();

	EXPECT_EQUALS(AssetManager::GetLightCount(), 1, "Bad Light count(AddLight)");

	const bool isEquals = AssetManager::GetLight(0) == newLight.get();
	EXPECT_EQUALS(isEquals, true, "Bad light");

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	newGameObject.reset();

	EXPECT_EQUALS(AssetManager::GetLightCount(), 0, "Bad Light count (RemoveLight)");

	END_TEST();
}