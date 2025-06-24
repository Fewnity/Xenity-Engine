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

	bool m_consoleMode = false;
	bool m_showLogs = true;
	bool m_showWarnings = true;
	bool m_showErrors = true;
	bool m_clearOnPlay = true;
	int m_lastHistoryCount = 0;
	float m_maxScrollSize;
	int m_needUpdateScrool = 0;
	std::shared_ptr<Socket> m_clientSocket;
	std::string m_totalClientText;
};

