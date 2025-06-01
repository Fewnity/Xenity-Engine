// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>
#include <engine/debug/debug_type.h>

class Socket;

class ConsoleMenu : public Menu
{
public:
	~ConsoleMenu();
	void Init() override;
	void Draw() override;

private:
	void OnNewDebug(const std::string& text, DebugType debugType);
	void OnPlay();
	bool consoleMode = false;
	bool showLogs = true;
	bool showWarnings = true;
	bool showErrors = true;
	bool clearOnPlay = true;
	int lastHistoryCount = 0;
	float maxScrollSize;
	int needUpdateScrool = 0;
	std::shared_ptr<Socket> clientSocket;
	std::string totalClientText;
};

