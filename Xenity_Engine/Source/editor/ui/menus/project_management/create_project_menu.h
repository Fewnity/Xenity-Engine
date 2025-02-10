// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

#include <editor/ui/menus/menu.h>

enum class CreateProjectError
{
	NO_ERROR_,
	ERROR_UNKNOWN,
	ERROR_PROJECT_ALREADY_EXISTS,
	ERROR_EMPTY_NAME,
	ERROR_EMPTY_FOLDER,
};

class CreateProjectMenu : public Menu
{
public:
	CreateProjectMenu();
	void Init() override;
	void Draw() override;

private:
	void DrawTitle();
	void DrawProjectPath();
	bool DrawSelectFolderButton();
	bool DrawProjectNameInput();
	void DrawError();
	void DrawCreateProjectButton();

	std::string projectName;
	std::string projectParentDir;
	CreateProjectError createProjectError = CreateProjectError::NO_ERROR_;
};

