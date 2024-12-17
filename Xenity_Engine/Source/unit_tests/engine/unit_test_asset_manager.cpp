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

bool AssetManagerTest::Start(std::string& errorOut)
{
	bool result = true;

	std::shared_ptr<GameObject> newGameObject = CreateGameObject();
	std::shared_ptr<Light> newLight = newGameObject->AddComponent<Light>();

	if (!Compare(AssetManager::GetLightCount(), 1)) 
	{
		errorOut += "Bad Light count (AddLight)\n";
		result = false;

	}
	bool isEquals = AssetManager::GetLight(0) == newLight.get();
	if (!Compare(isEquals, true)) 
	{
		errorOut += "Bad Light\n";
		result = false;
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	newGameObject.reset();
	if (!Compare(AssetManager::GetLightCount(), 0)) 
	{
		errorOut += "Bad Light count (RemoveLight)\n";
		result = false;
	}

	return result;
}