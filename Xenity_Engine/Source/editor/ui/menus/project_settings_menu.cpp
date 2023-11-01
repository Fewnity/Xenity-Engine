#include "project_settings_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../../engine/asset_management/project_manager.h"

void ProjectSettingsMenu::Init()
{
}

void ProjectSettingsMenu::Draw()
{
	bool visible = ImGui::Begin("Project Settings", &EditorUI::showProjectsSettings, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		EditorUI::DrawMap(ProjectManager::GetProjetSettingsReflection());

		if (ImGui::Button("Save"))
		{
			ProjectManager::SaveProjectSettings();
		}
	}

	ImGui::End();
}
