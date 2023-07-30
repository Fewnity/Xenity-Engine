#include "game_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"

void GameMenu::Init()
{
}

void GameMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string windowName = "Game " + std::to_string(Graphics::framebufferSize.x) + "x" + std::to_string(Graphics::framebufferSize.y) + "###Game";
	ImGui::Begin(windowName.c_str());
	ImVec2 size = ImGui::GetContentRegionAvail();
	Graphics::ChangeFrameBufferSize(Vector2Int(size.x, size.y));
	ImGui::Image((ImTextureID)Graphics::framebufferTexture, size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar();
}
