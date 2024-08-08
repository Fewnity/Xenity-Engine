// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include "menu.h"

class File;

class EngineAssetManagerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	std::vector<std::shared_ptr<File>> engineAssetsFiles;
	std::vector<uint64_t> ids;
	std::vector<uint64_t> oldIds;
};

