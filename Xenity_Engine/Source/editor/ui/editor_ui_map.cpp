// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

InputButtonState EditorUI::DrawInputButton(const std::string& inputName, const std::string& buttonText, bool addUnbindButton)
{
	InputButtonState returnValue = InputButtonState::Null;
	DrawInputTitle(inputName);
	float w = ImGui::GetContentRegionAvail().x;
	if (addUnbindButton)
	{
		w -= 25 * uiScale;
	}
	ImGui::BeginGroup();
	const std::string id = buttonText + GenerateItemId();
	if (ImGui::Button(id.c_str(), ImVec2(w, 0)))
	{
		returnValue = InputButtonState::OpenAssetMenu;
	}
	if (addUnbindButton)
	{
		ImGui::SameLine();
		const std::string id2 = "X" + GenerateItemId();
		if (ImGui::Button(id2.c_str()))
		{
			returnValue = InputButtonState::ResetValue;
		}
	}
	ImGui::EndGroup();
	return returnValue;
}

#endif // #if defined(EDITOR)