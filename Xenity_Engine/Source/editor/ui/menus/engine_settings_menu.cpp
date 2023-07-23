#include "engine_settings_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"

void EngineSettingsMenu::Init()
{
}

void EngineSettingsMenu::Draw()
{
	ImGui::Begin("Enigne Settings", &EditorUI::showEngineSettings, ImGuiWindowFlags_NoCollapse);

	ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useProfiler);
	ImGui::SameLine();
	ImGui::TextWrapped("Use Profiler");

	ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useLighting);
	ImGui::SameLine();
	ImGui::TextWrapped("Use Lighting");

	ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useDebugger);
	ImGui::SameLine();
	ImGui::TextWrapped("Use Debugger (Print logs in the console and in the file)");

	ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useOnlineDebugger);
	ImGui::SameLine();
	ImGui::TextWrapped("Use Online Debugger (Print logs to an online console)");

	if (ImGui::Button("Save"))
	{
		Debug::PrintWarning("(DrawEngineSettings [Save]) Unimplemented button");
	}

	ImGui::End();
}
