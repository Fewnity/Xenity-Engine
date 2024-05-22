// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <vector>

struct ProjectListItem;

class SelectProjectMenu : public Menu
{
public:
	SelectProjectMenu();
	void Init() override;
	void Draw() override;
private:
	void OnLoadButtonClick();
	void DrawProjectsList();
	std::vector<ProjectListItem> projectsList;
};

