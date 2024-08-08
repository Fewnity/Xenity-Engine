// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"

class ConsoleMenu : public Menu
{
public:
	~ConsoleMenu();
	void Init() override;
	void Draw() override;

private:
	void OnNewDebug();
	void OnPlay();
	bool consoleMode = false;
	bool showLogs = true;
	bool showWarnings = true;
	bool showErrors = true;
	bool clearOnPlay = true;
	int lastHistoryCount = 0;
	float maxScrollSize;
	int needUpdateScrool = 0;
};

