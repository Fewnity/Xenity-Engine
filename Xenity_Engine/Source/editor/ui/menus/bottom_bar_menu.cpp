#include "bottom_bar_menu.h"
#include <imgui/imgui.h>
#include <engine/debug/debug.h>

void BottomBarMenu::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 30));
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 32));
	ImGui::Begin("bottom_bar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	if (Debug::lastDebugMessageHistoryIndex != -1)
	{
		const DebugHistory& history = Debug::debugMessageHistory[Debug::lastDebugMessageHistoryIndex];

		ImVec4 color = ImVec4(1, 1, 1, 1);
		if (history.type == DebugType::Warning)
		{
			color = ImVec4(1, 1, 0, 1);
		}
		else if (history.type == DebugType::Error)
		{
			color = ImVec4(1, 0, 0, 1);
		}

		ImVec2 txtSize = ImGui::CalcTextSize(Debug::debugMessageHistory[Debug::lastDebugMessageHistoryIndex].message.c_str());
		ImGui::SetCursorPosX(4);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y / 2 - txtSize.y / 2);
		ImGui::TextColored(color, "%s", Debug::debugMessageHistory[Debug::lastDebugMessageHistoryIndex].message.c_str());
		// Find a way to force a dock to change tab
		/*if (ImGui::IsItemClicked())
		{
			Editor::GetMenu<ConsoleMenu>()->Focus();
		}*/
	}
	ImGui::End();
}
