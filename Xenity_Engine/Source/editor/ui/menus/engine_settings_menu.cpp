#include "engine_settings_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/engine_settings.h>

void EngineSettingsMenu::Init()
{
}

bool EngineSettingsMenu::DrawSelectFolderButton(std::string& path)
{
	// Draw select folder button
	bool projectFolderChanged = false;
	std::string buttonId = "Select a folder" + std::string(EditorUI::GenerateItemId());
	if (ImGui::Button(buttonId.c_str()))
	{
		std::string folder = EditorUI::OpenFolderDialog("Select a folder");
		if (!folder.empty())
		{
			path = folder;
			projectFolderChanged = true;
		}
	}
	return projectFolderChanged;
}

void EngineSettingsMenu::Draw()
{
	bool visible = ImGui::Begin("Engine Settings", &EditorUI::showEngineSettings, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useProfiler);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Profiler");

		ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Debugger (Print logs in the console and in the file)");

		ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useOnlineDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Online Debugger (Print logs to an online console)");

		bool changed = false;

		std::string tempEngineProjectPath;
		ImGui::Text("Engine project location: %s", EngineSettings::engineProjectPath.c_str());
		changed = DrawSelectFolderButton(tempEngineProjectPath);
		if (changed) 
		{
			EngineSettings::engineProjectPath = tempEngineProjectPath;
		}

		std::string tempCompilerPath;
		ImGui::Text("Compiler location: %s", EngineSettings::compilerPath.c_str());
		changed = DrawSelectFolderButton(tempCompilerPath);
		if (changed)
		{
			EngineSettings::compilerPath = tempCompilerPath;
		}

		std::string tempPpssppExePath;
		ImGui::Text("PPSSPP location: %s", EngineSettings::ppssppExePath.c_str());
		changed = DrawSelectFolderButton(tempPpssppExePath);
		if (changed)
		{
			EngineSettings::ppssppExePath = tempPpssppExePath + "PPSSPPWindows64.exe";
		}

		if (ImGui::Button("Save"))
		{
			EngineSettings::SaveEngineSettings();
		}
	}
	ImGui::End();
}
