// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
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
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<Scene> MakeScene();
private:
};

