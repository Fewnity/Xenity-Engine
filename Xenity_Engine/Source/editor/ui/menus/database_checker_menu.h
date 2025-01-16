// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/file_system/data_base/integrity_state.h>
#include "menu.h"

class FileDataBase;

class DataBaseCheckerMenu : public Menu
{
public:
	DataBaseCheckerMenu();

	void Init() override;
	void Draw() override;

private:
	bool wrongDbLoaded = false;
	bool loaded = false;
	std::unique_ptr<FileDataBase> db = nullptr;
	IntegrityState integrityState = IntegrityState::Integrity_Ok;
};

