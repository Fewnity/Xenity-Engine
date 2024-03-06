#include "project_settings_menu.h"
#include <imgui/imgui.h>

#include "editor/ui/editor_ui.h"
#include <engine/asset_management/project_manager.h>
#include <editor/command/command_manager.h>

void ProjectSettingsMenu::Init()
{
}

void ProjectSettingsMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Project Settings", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		std::shared_ptr<Command> command = nullptr;
		std::shared_ptr<FileReference> emptyPtr = std::make_shared<FileReference>();
		EditorUI::DrawReflectiveData(ProjectManager::projectSettings.GetReflectiveData(), command, emptyPtr);
		if (command)
		{
			CommandManager::AddCommand(command);
			command->Execute();
		}
		if (ImGui::Button("Save"))
		{
			ProjectManager::SaveProjectSettings();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
