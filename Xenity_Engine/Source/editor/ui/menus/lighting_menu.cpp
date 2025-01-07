// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "lighting_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/graphics/graphics.h>

void LightingMenu::Init()
{
}

void LightingMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Lighting", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		bool changed = false;
		ImGui::Text("Lighting");
		ImGui::Separator();

		ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(AssetPlatform::AP_Standalone);
		EditorUI::DrawReflectiveData(reflectiveDataToDraw, Graphics::s_settings.GetReflectiveData(), nullptr);
		if (reflectiveDataToDraw.command)
		{
			CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
			Graphics::OnLightingSettingsReflectionUpdate();
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
