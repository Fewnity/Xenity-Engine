// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "about_menu.h"

#include <imgui/imgui.h>
#include <editor/editor.h>

void AboutMenu::Init()
{
}

void AboutMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("About Xenity Engine", &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		ImGui::Text("Version: 0.1");
		ImGui::Text("Made by Fewnity with love <3");

		if(ImGui::Button("Credits"))
		{
			Editor::OpenLinkInWebBrowser("https://fewnity.github.io/Xenity-Engine/credits.html");
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
