#include "compiling_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/asset_management/project_manager.h>

void CompilingMenu::Init()
{
}

void CompilingMenu::Draw()
{
	if (popupState == Opening)
	{
		popupState = Closing;
		ImGui::OpenPopup("Compiling...");
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	// Draw compiling popup
	if (ImGui::BeginPopupModal("Compiling...", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
	{
		ImGui::Text("Compiling game...");
		if (popupState == Closed)
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void CompilingMenu::OpenPopup()
{
	popupState = Opening;
}

void CompilingMenu::ClosePopup()
{
	popupState = Closed;
}
