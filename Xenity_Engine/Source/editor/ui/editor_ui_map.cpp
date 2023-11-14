#if defined(EDITOR)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

void EditorUI::DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
}

void EditorUI::DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputInt(inputId.c_str(), &value, 0, 0);
}

int EditorUI::DrawInputButton(const std::string& inputName, const std::string& text, bool addUnbindButton)
{
	int returnValue = 0;
	DrawInputTitle(inputName);
	float w = ImGui::GetContentRegionAvail().x;
	if (addUnbindButton)
	{
		w -= 25 * uiScale;
	}
	ImGui::BeginGroup();
	std::string id = text + GenerateItemId();
	if (ImGui::Button(id.c_str(), ImVec2(w, 0)))
	{
		returnValue = 1;
	}
	if (addUnbindButton)
	{
		ImGui::SameLine();
		std::string id2 = "X" + GenerateItemId();
		if (ImGui::Button(id2.c_str()))
		{
			returnValue = 2;
		}
	}
	ImGui::EndGroup();
	return returnValue;
}

#endif // #if defined(EDITOR)