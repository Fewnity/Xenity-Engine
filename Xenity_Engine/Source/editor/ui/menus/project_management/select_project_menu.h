// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>

#include <vector>
#include <thread>

#include <engine/asset_management/project_list_item.h>
#include <engine/project_management/project_errors.h>

struct ImVec2;

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
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const uint32_t& bg_col, const uint32_t& fg_col);

	std::vector<ProjectListItem> m_projectsList;
	ProjectListItem* m_selectedProject = nullptr;
	ProjectListItem* m_projectToLoad = nullptr;
	std::thread m_loadingThread;
};

