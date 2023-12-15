#include "engine_settings_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/engine_settings.h>
#include <engine/debug/debug.h>

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
		std::string folder = EditorUI::OpenFolderDialog("Select a folder", "");
		if (!folder.empty())
		{
			path = folder;
			projectFolderChanged = true;
		}
	}
	return projectFolderChanged;
}

bool EngineSettingsMenu::DrawCompilerOptions()
{
	bool settingsChanged = false;
	bool valueChanged = false;

	ImGui::Separator();
	ImGui::Text("Compiler Options:");
	ImGui::Separator();

	std::string tempEngineProjectPath;
	ImGui::Text("Engine project location: %s", EngineSettings::engineProjectPath.c_str());
	valueChanged = DrawSelectFolderButton(tempEngineProjectPath);
	if (valueChanged)
	{
		EngineSettings::engineProjectPath = tempEngineProjectPath;
		settingsChanged = true;
	}

	std::string tempCompilerPath;
	ImGui::Text("Compiler location: %s", EngineSettings::compilerPath.c_str());
	valueChanged = DrawSelectFolderButton(tempCompilerPath);
	if (valueChanged)
	{
		EngineSettings::compilerPath = tempCompilerPath;
		settingsChanged = true;
	}

	std::string tempPpssppExePath;
	ImGui::Text("PPSSPP location: %s", EngineSettings::ppssppExePath.c_str());
	valueChanged = DrawSelectFolderButton(tempPpssppExePath);
	if (valueChanged)
	{
		EngineSettings::ppssppExePath = tempPpssppExePath + "PPSSPPWindows64.exe";
		settingsChanged = true;
	}

	valueChanged = ImGui::Checkbox("Compile On Code Changed", &EngineSettings::compileOnCodeChanged);
	if (valueChanged)
		settingsChanged = true;

	valueChanged = ImGui::Checkbox("Compile On Project Opened", &EngineSettings::compileWhenOpeningProject);
	if (valueChanged)
		settingsChanged = true;

	return settingsChanged;
}

void EngineSettingsMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 350), ImGuiCond_FirstUseEver);
	bool visible = ImGui::Begin("Engine Settings", 0, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		bool settingsChanged = false;
		bool valueChanged = false;
		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useProfiler);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Profiler");
		if (valueChanged)
			settingsChanged = true;
		
		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Debugger (Print logs in the console and in the file)");
		if (valueChanged)
			settingsChanged = true;

		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::useOnlineDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Online Debugger (Print logs to an online console)");
		if (valueChanged)
			settingsChanged = true;

		if (DrawCompilerOptions()) 
		{
			settingsChanged = true;
		}

		if (settingsChanged)
		{
			EngineSettings::SaveEngineSettings();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
