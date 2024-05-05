#include "engine_settings_menu.h"

#include <imgui/imgui.h>
#include <editor/editor.h>
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
	const std::string buttonId = "Select a folder" + std::string(EditorUI::GenerateItemId());
	if (ImGui::Button(buttonId.c_str()))
	{
		std::string unsued;
		std::string pathToOpen = path;
		Editor::SeparateFileFromPath(path, pathToOpen, unsued);
		const std::string folder = EditorUI::OpenFolderDialog("Select a folder", pathToOpen);
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

	std::string tempCompilerPath = EngineSettings::values.compilerPath;
	ImGui::Text("Compiler location: %s", EngineSettings::values.compilerPath.c_str());
	valueChanged = DrawSelectFolderButton(tempCompilerPath);
	if (valueChanged)
	{
		EngineSettings::values.compilerPath = tempCompilerPath;
		settingsChanged = true;
	}

	std::string tempPpssppExePath = EngineSettings::values.ppssppExePath;
	ImGui::Text("PPSSPP location: %s", EngineSettings::values.ppssppExePath.c_str());
	valueChanged = DrawSelectFolderButton(tempPpssppExePath);
	if (valueChanged)
	{
		EngineSettings::values.ppssppExePath = tempPpssppExePath + "PPSSPPWindows64.exe";
		settingsChanged = true;
	}

	std::string tempDockerExePath = EngineSettings::values.dockerExePath;
	ImGui::Text("Docker location: %s", EngineSettings::values.dockerExePath.c_str());
	valueChanged = DrawSelectFolderButton(tempDockerExePath);
	if (valueChanged)
	{
		EngineSettings::values.dockerExePath = tempDockerExePath + "Docker Desktop.exe";
		settingsChanged = true;
	}

	valueChanged = ImGui::Checkbox("Compile On Code Changed", &EngineSettings::values.compileOnCodeChanged);
	if (valueChanged)
		settingsChanged = true;

	valueChanged = ImGui::Checkbox("Compile On Project Opened", &EngineSettings::values.compileWhenOpeningProject);
	if (valueChanged)
		settingsChanged = true;

	return settingsChanged;
}

void EngineSettingsMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(0, 350), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Engine Settings", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		bool settingsChanged = false;
		bool valueChanged = false;
		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::values.useProfiler);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Profiler");
		if (valueChanged)
			settingsChanged = true;
		
		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::values.useDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Debugger (Print logs in the console and in the file)");
		if (valueChanged)
			settingsChanged = true;

		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::values.useOnlineDebugger);
		ImGui::SameLine();
		ImGui::TextWrapped("Use Online Debugger (Print logs to an online console)");
		if (valueChanged)
			settingsChanged = true;

		valueChanged = EditorUI::DrawInput("Backbground color",EngineSettings::values.backbgroundColor);
		if (valueChanged)
			settingsChanged = true;
		valueChanged = EditorUI::DrawInput("Secondary color", EngineSettings::values.secondaryColor);
		if (valueChanged)
			settingsChanged = true;
		valueChanged = EditorUI::DrawInput("Play tint color", EngineSettings::values.playTintColor);
		if (valueChanged)
			settingsChanged = true;

		valueChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EngineSettings::values.isPlayTintAdditive);
		ImGui::SameLine();
		ImGui::TextWrapped("Is Play Tint Additive");
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
