// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "update_available_menu.h"

#include <imgui/imgui.h>
#include <editor/editor.h>

void UpdateAvailableMenu::Init()
{
}

void UpdateAvailableMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Update Available!", &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		ImGui::Text("A new version of the engine is available!");
		ImGui::Text("Please download the new version to get the latest features and bug fixes.");
		if (ImGui::Button("View GitHub page"))
		{
			Editor::OpenLinkInWebBrowser("https://github.com/Fewnity/Xenity-Engine/releases");
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
