// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class Icon : public FileReference, public Reflective
{
protected:
	friend class ProjectManager;

	ReflectiveData GetReflectiveData() override;

	static std::shared_ptr<Icon> MakeIcon();
};

