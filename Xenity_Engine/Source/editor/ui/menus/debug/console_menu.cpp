// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "console_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/ui/utils/menu_builder.h>

#include <engine/debug/debug.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/network/network.h>

ConsoleMenu::~ConsoleMenu()
{
	Debug::GetOnDebugLogEvent().Unbind(&ConsoleMenu::OnNewDebug, this);
	GameplayManager::GetOnPlayEvent().Unbind(&ConsoleMenu::OnPlay, this);
}

void ConsoleMenu::Init()
{
	Debug::GetOnDebugLogEvent().Bind(&ConsoleMenu::OnNewDebug, this);
	GameplayManager::GetOnPlayEvent().Bind(&ConsoleMenu::OnPlay, this);
}

void ConsoleMenu::OnNewDebug(const std::string& text, DebugType debugType)
{
	m_needUpdateScrool = 1;
}

void ConsoleMenu::OnPlay()
{
	if (m_clearOnPlay)
		Debug::ClearDebugLogs();
}

void ConsoleMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	const std::string windowName = "Console###Console" + std::to_string(id);
	const bool visible = ImGui::Begin(windowName.c_str(), &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		const ImVec2 startCusorPos = ImGui::GetCursorPos();

		ImGui::SetCursorPosY(startCusorPos.y * 2);
		const size_t historyCount = Debug::s_debugMessageHistory.size();

		if (m_needUpdateScrool != 0)
			m_needUpdateScrool++;

		if (m_needUpdateScrool >= 6)
		{
			m_needUpdateScrool = 0;
			ImGui::SetNextWindowScroll(ImVec2(-1, m_maxScrollSize));
		}

		ImGui::BeginChild("ConsoleMenuChild");
		if (m_consoleMode)
		{
			ImGui::Text("%s", Debug::GetDebugString().c_str());
			RightClickMenu rightClickMenu = RightClickMenu("ConsoleMenuRightClick");
			RightClickMenuState rightClickState = rightClickMenu.Check(false, false);
			if (rightClickState != RightClickMenuState::Closed)
			{
				rightClickMenu.AddItem("Clear", []() { Debug::ClearDebugLogs(); });
			}
			rightClickMenu.Draw();
		}
		else
		{
			for (size_t i = 0; i < historyCount; i++)
			{
				const DebugHistory& history = Debug::s_debugMessageHistory[i];

				ImVec4 color = ImVec4(1, 1, 1, 1);
				if (history.type == DebugType::Log)
				{
					if (!m_showLogs)
						continue;
				}
				else if (history.type == DebugType::Warning)
				{
					if (!m_showWarnings)
						continue;

					color = ImVec4(1, 1, 0, 1);
				}
				else if (history.type == DebugType::Error)
				{
					if (!m_showErrors)
						continue;

					color = ImVec4(1, 0, 0, 1);
				}

				ImGui::TextColored(color, "[%d] %s", history.count, history.message.c_str());
				RightClickMenu rightClickMenu = RightClickMenu("ConsoleItemRightClickMenu" + std::to_string(i) + "," + std::to_string(id));
				RightClickMenuState rightClickState = rightClickMenu.Check(false, false);
				if (rightClickState != RightClickMenuState::Closed)
				{
					rightClickMenu.AddItem("Copy", [&history]() {
						ImGui::SetClipboardText(history.message.c_str());
						});
				}
				rightClickMenu.Draw();
			}
		}
		if (m_needUpdateScrool == 5)
		{
			if (ImGui::GetScrollY() != m_maxScrollSize)
			{
				m_needUpdateScrool = 0;
			}
			m_maxScrollSize = ImGui::GetScrollMaxY();
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(startCusorPos);
		ImGui::BeginChild("ConsoleMenuChild2", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
		std::string modeButtonText = "Console mode";
		if (m_consoleMode)
			modeButtonText = "List mode";

		if (ImGui::Button(modeButtonText.c_str()))
		{
			m_consoleMode = !m_consoleMode;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			Debug::ClearDebugLogs();
		}
		ImGui::SameLine();
		EditorUI::SetButtonColor(m_clearOnPlay);
		if (ImGui::Button("Clear on play"))
		{
			m_clearOnPlay = !m_clearOnPlay;
		}
		EditorUI::EndButtonColor();

		if (!m_consoleMode)
		{
			ImGui::SameLine();
			EditorUI::SetButtonColor(m_showLogs);
			if (ImGui::Button("Show Logs"))
			{
				m_showLogs = !m_showLogs;
			}
			EditorUI::EndButtonColor();

			ImGui::SameLine();
			EditorUI::SetButtonColor(m_showWarnings);
			if (ImGui::Button("Show Warnings"))
			{
				m_showWarnings = !m_showWarnings;
			}
			EditorUI::EndButtonColor();

			ImGui::SameLine();
			EditorUI::SetButtonColor(m_showErrors);
			if (ImGui::Button("Show Errors"))
			{
				m_showErrors = !m_showErrors;
			}
			EditorUI::EndButtonColor();
		}

		//ImGui::SameLine();
		/*if (ImGui::Button("Connect to client"))
		{
			clientSocket = NetworkManager::GetClientSocket();
			totalClientText = "";
		}

		if (clientSocket)
		{
			const std::string clientData = clientSocket->GetIncommingData();
			if (!clientData.empty())
			{
				totalClientText += clientData;
			}

			size_t startPos = totalClientText.find_first_of('{');
			size_t endPos = totalClientText.find_first_of('}');
			while (startPos != -1 && startPos < endPos)
			{
				Debug::Print("Client: " + totalClientText.substr(startPos, endPos+1));
				totalClientText = totalClientText.substr(endPos+1);

				startPos = totalClientText.find_first_of('{');
				endPos = totalClientText.find_first_of('}');
			}
		}*/

		ImGui::EndChild();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
