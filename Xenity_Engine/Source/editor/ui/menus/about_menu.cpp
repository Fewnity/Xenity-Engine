// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "about_menu.h"

#include <imgui/imgui.h>

void AboutMenu::Init()
{
}

void AboutMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("About Xenity Engine", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		ImGui::Text("Version: 0.1");
		ImGui::Text("Made by Fewnity with love <3");

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
