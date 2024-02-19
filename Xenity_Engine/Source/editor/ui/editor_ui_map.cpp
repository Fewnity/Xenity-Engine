#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>

InputButtonState EditorUI::DrawInputButton(const std::string& inputName, const std::string& text, bool addUnbindButton)
{
	InputButtonState returnValue = InputButtonState::Null;
	DrawInputTitle(inputName);
	float w = ImGui::GetContentRegionAvail().x;
	if (addUnbindButton)
	{
		w -= 25 * uiScale;
	}
	ImGui::BeginGroup();
	const std::string id = text + GenerateItemId();
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