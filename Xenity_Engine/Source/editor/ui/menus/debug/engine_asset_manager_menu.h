// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <editor/ui/menus/menu.h>

class File;

class EngineAssetManagerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	std::vector<std::shared_ptr<File>> m_engineAssetsFiles;
	std::vector<uint64_t> m_ids;
	std::vector<uint64_t> m_oldIds;
};

