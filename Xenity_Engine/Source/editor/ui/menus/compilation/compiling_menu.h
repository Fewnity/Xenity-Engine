// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>

struct CompilerParams;

enum class CompilingPupopState
{
	Closed,
	Closing,
	Opening
};

class CompilingMenu : public Menu
{
public:
	~CompilingMenu();
	void Init() override;
	void Draw() override;
	void OpenPopup(CompilerParams params);
	void ClosePopup(CompilerParams params, bool result);

private:
	CompilingPupopState m_popupState = CompilingPupopState::Closed;
};

