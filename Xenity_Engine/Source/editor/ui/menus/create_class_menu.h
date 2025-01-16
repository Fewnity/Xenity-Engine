// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <string>

class CreateClassMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Reset();
	void SetFolderPath(const std::string& path);

private:
	std::string className = "";
	std::string fileName = "";
	std::string folderPath = "";
	bool fileNameChanged = false;
	void SetFileNameFromClassName();
	void CreateFiles();
};

