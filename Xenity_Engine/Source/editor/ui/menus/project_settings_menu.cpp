// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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

		ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw();
		EditorUI::DrawReflectiveData(reflectiveDataToDraw, ProjectManager::projectSettings.GetReflectiveData(), nullptr);
		if (reflectiveDataToDraw.command)
		{
			CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
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
