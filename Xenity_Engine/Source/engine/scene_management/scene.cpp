// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "scene.h"

#include <engine/asset_management/asset_manager.h>

Scene::Scene()
{
}

ReflectiveData Scene::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Scene::GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<Scene> Scene::MakeScene()
{
	std::shared_ptr<Scene> newFileRef = std::make_shared<Scene>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}