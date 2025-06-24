// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/file_system/data_base/integrity_state.h>
#include <editor/ui/menus/menu.h>

class FileDataBase;

class DataBaseCheckerMenu : public Menu
{
public:
	DataBaseCheckerMenu();
	~DataBaseCheckerMenu();

	void Init() override;
	void Draw() override;

private:
	bool m_wrongDbLoaded = false;
	bool m_loaded = false;
	std::unique_ptr<FileDataBase> m_db = nullptr;
	IntegrityState m_integrityState = IntegrityState::Integrity_Ok;
};

