// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"

#include <vector>

#include <engine/asset_management/project_list_item.h>
#include <engine/project_management/project_errors.h>

class SelectProjectMenu : public Menu
{
public:
	SelectProjectMenu();
	void Init() override;
	void Draw() override;
private:
	void OnLoadButtonClick();
	void DrawProjectsList();
	void DeleteProject(size_t projectIndex, bool deleteFiles);
	void ShowProjectError(ProjectLoadingErrors error);

	std::vector<ProjectListItem> projectsList;
	ProjectListItem* selectedProject = nullptr;
};

