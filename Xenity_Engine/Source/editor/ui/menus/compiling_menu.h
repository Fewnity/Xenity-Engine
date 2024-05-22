// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"

class CompilerParams;

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
	CompilingPupopState popupState = CompilingPupopState::Closed;
};

