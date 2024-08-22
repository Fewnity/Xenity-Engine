// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "console_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/ui/utils/menu_builder.h>

#include <engine/debug/debug.h>
#include <engine/game_elements/gameplay_manager.h>

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

void ConsoleMenu::OnNewDebug() 
{
	needUpdateScrool = 1;
}

void ConsoleMenu::OnPlay()
{
	if(clearOnPlay)
		Debug::ClearDebugLogs();
}

void ConsoleMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	const std::string windowName = "Console###Console" + std::to_string(id);
	const bool visible = ImGui::Begin(windowName.c_str(), &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		const ImVec2 startCusorPos = ImGui::GetCursorPos();

		ImGui::SetCursorPosY(startCusorPos.y * 2);
		const size_t historyCount = Debug::s_debugMessageHistory.size();
		
		if (needUpdateScrool != 0)
			needUpdateScrool++;

		if (needUpdateScrool >= 6)
		{
			needUpdateScrool = 0;
			ImGui::SetNextWindowScroll(ImVec2(-1, maxScrollSize));
		}

		ImGui::BeginChild("ConsoleMenuChild");
		if (consoleMode)
		{
			ImGui::Text("%s", Debug::GetDebugString().c_str());
			RightClickMenu rightClickMenu = RightClickMenu("ConsoleMenuRightClick");
			RightClickMenuState rightClickState = rightClickMenu.Check(false);
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

				ImVec4 color = ImVec4(1,1,1,1);
				if (history.type == DebugType::Log)
				{
					if (!showLogs)
						continue;
				}
				else if (history.type == DebugType::Warning) 
				{
					if (!showWarnings)
						continue;

					color = ImVec4(1, 1, 0, 1);
				}
				else if (history.type == DebugType::Error) 
				{
					if (!showErrors)
						continue;

					color = ImVec4(1, 0, 0, 1);
				}

				ImGui::TextColored(color, "[%d] %s", history.count, history.message.c_str());
				RightClickMenu rightClickMenu = RightClickMenu("ConsoleItemRightClickMenu" + std::to_string(i) + "," + std::to_string(id));
				RightClickMenuState rightClickState = rightClickMenu.Check(false);
				if (rightClickState != RightClickMenuState::Closed) 
				{
					rightClickMenu.AddItem("Copy", [&history]() { 
							ImGui::SetClipboardText(history.message.c_str());
					});
				}
				rightClickMenu.Draw();
			}
		}
		if (needUpdateScrool == 5) 
		{
			if (ImGui::GetScrollY() != maxScrollSize)
			{
				needUpdateScrool = 0;
			}
			maxScrollSize = ImGui::GetScrollMaxY();
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(startCusorPos);
		ImGui::BeginChild("ConsoleMenuChild2", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
		std::string modeButtonText = "Console mode";
		if (consoleMode)
			modeButtonText = "List mode";

		if (ImGui::Button(modeButtonText.c_str()))
		{
			consoleMode = !consoleMode;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			Debug::ClearDebugLogs();
		}
		ImGui::SameLine();
		EditorUI::SetButtonColor(clearOnPlay);
		if (ImGui::Button("Clear on play"))
		{
			clearOnPlay = !clearOnPlay;
		}
		EditorUI::EndButtonColor();

		if (!consoleMode) 
		{
			ImGui::SameLine();
			EditorUI::SetButtonColor(showLogs);
			if (ImGui::Button("Show Logs"))
			{
				showLogs = !showLogs;
			}
			EditorUI::EndButtonColor();

			ImGui::SameLine();
			EditorUI::SetButtonColor(showWarnings);
			if (ImGui::Button("Show Warnings"))
			{
				showWarnings = !showWarnings;
			}
			EditorUI::EndButtonColor();

			ImGui::SameLine();
			EditorUI::SetButtonColor(showErrors);
			if (ImGui::Button("Show Errors"))
			{
				showErrors = !showErrors;
			}
			EditorUI::EndButtonColor();
		}
		ImGui::EndChild();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
