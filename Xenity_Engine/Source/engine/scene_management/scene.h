// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/file_system/file_reference.h>
#include <string>

/**
* @brief Scene file
*/
class API Scene : public FileReference, public Reflective
{
public:
	Scene();

protected:
	friend class ProjectManager;

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	static std::shared_ptr<Scene> MakeScene();
};

