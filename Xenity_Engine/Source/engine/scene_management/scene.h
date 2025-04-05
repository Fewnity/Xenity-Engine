// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/file_system/file_reference.h>

/**
* @brief Scene file
*/
class API Scene : public FileReference
{
public:
	Scene();

protected:
	friend class ProjectManager;

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	[[nodiscard]] static std::shared_ptr<Scene> MakeScene();
};

