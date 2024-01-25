#include "console_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/graphics.h>
#include <engine/debug/debug.h>

void ConsoleMenu::Init()
{
}

void ConsoleMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Console", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		ImVec2 startCusorPos = ImGui::GetCursorPos();

		ImGui::SetCursorPosY(startCusorPos.y * 2);
		int historyCount = Debug::debugMessageHistory.size();
		ImGui::BeginChild("ConsoleMenuChild");
		if (consoleMode)
		{
			ImGui::Text("%s", Debug::GetDebugString().c_str());
		}
		else
		{
			for (int i = 0; i < historyCount; i++)
			{
				DebugHistory& history = Debug::debugMessageHistory[i];

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
			}
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
