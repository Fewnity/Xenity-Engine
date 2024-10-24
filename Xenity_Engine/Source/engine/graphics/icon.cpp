// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "icon.h"

#include <engine/asset_management/asset_manager.h>

ReflectiveData Icon::GetReflectiveData()
{
    return ReflectiveData();
}

std::shared_ptr<Icon> Icon::MakeIcon()
{
	const std::shared_ptr<Icon> newFileRef = std::make_shared<Icon>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

