// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>
#include <string>

class CreateClassMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Reset();
	void SetFolderPath(const std::string& path);

private:
	void SetFileNameFromClassName();
	void CreateFiles();

	std::string m_className = "";
	std::string m_fileName = "";
	std::string m_folderPath = "";
	bool m_fileNameChanged = false;
};

