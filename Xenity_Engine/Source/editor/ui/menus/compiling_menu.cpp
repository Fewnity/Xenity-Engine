#include "compiling_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../../engine/asset_management/project_manager.h"

void CompilingMenu::Init()
{
}

void CompilingMenu::Draw()
{
	if (popupState == 2)
	{
		popupState = 1;
		ImGui::OpenPopup("Compiling...");
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Compiling...", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
	{
		ImGui::Text("Compiling game...");
		if (popupState == 0)
		{
			ImGui::CloseCurrentPopup();
		}
		// Draw popup contents.
		ImGui::EndPopup();
	}
}

void CompilingMenu::OpenPopup()
{
	popupState = 2;
}

void CompilingMenu::ClosePopup()
{
	popupState = 0;
}
