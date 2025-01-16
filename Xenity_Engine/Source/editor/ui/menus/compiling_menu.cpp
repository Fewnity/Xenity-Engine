// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "compiling_menu.h"

#include <imgui/imgui.h>

#include <editor/compiler.h>

CompilingMenu::~CompilingMenu()
{
	Compiler::GetOnCompilationStartedEvent().Unbind(&CompilingMenu::OpenPopup, this);
	Compiler::GetOnCompilationEndedEvent().Unbind(&CompilingMenu::ClosePopup, this);
}

void CompilingMenu::Init()
{
	Compiler::GetOnCompilationStartedEvent().Bind(&CompilingMenu::OpenPopup, this);
	Compiler::GetOnCompilationEndedEvent().Bind(&CompilingMenu::ClosePopup, this);
}

void CompilingMenu::Draw()
{
	if (popupState == CompilingPupopState::Opening)
	{
		popupState = CompilingPupopState::Closing;
		ImGui::OpenPopup("Compiling...");
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	// Draw compiling popup
	if (ImGui::BeginPopupModal("Compiling...", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
	{
		ImGui::Text("Compiling game...");
		if (Compiler::GetCompilationMethod() == CompilationMethod::DOCKER)
		{
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				Compiler::CancelCompilation();
			}
		}
		if (popupState == CompilingPupopState::Closed)
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void CompilingMenu::OpenPopup(CompilerParams params)
{
	popupState = CompilingPupopState::Opening;
}

void CompilingMenu::ClosePopup(CompilerParams params, bool result)
{
	popupState = CompilingPupopState::Closed;
}
