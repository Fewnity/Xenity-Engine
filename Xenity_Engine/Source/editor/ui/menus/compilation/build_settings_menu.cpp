// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "build_settings_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/compilation/compiler.h>
#include <editor/command/command_manager.h>

#include <engine/engine_settings.h>
#include <engine/file_system/file_system.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/graphics/texture/texture_default.h>
#include <editor/ui/editor_icons.h>

using json = nlohmann::json;

std::vector<BuildPlatform> BuildSettingsMenu::buildPlatforms;

void BuildSettingsMenu::Init()
{
	onSettingChangedEvent = new Event<>();

	///// Create all build platforms

	BuildPlatform windowsPlatform = BuildPlatform();
	windowsPlatform.name = "Windows";
	windowsPlatform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_Windows];
	windowsPlatform.isSupported = true;
	windowsPlatform.supportBuildAndRun = true;
	windowsPlatform.supportBuildAndRunOnHardware = false;
	windowsPlatform.platform = Platform::P_Windows;
	windowsPlatform.settings = std::make_shared<PlatformSettingsWindows>(onSettingChangedEvent);

	BuildPlatform linuxPlatform = BuildPlatform();
	linuxPlatform.name = "Linux";
	linuxPlatform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_Linux];
	linuxPlatform.isSupported = false;
	linuxPlatform.supportBuildAndRun = false;
	linuxPlatform.supportBuildAndRunOnHardware = false;
	linuxPlatform.platform = Platform::P_Linux;
	linuxPlatform.settings = std::make_shared<PlatformSettingsWindows>(onSettingChangedEvent);

	BuildPlatform pspPlatform = BuildPlatform();
	pspPlatform.name = "PSP";
	pspPlatform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_PSP];
	pspPlatform.isSupported = true;
	pspPlatform.supportBuildAndRun = true;
	pspPlatform.supportBuildAndRunOnHardware = false;
	pspPlatform.platform = Platform::P_PSP;
	pspPlatform.settings = std::make_shared<PlatformSettingsPSP>(onSettingChangedEvent);

	BuildPlatform psvitaPlatform = BuildPlatform();
	psvitaPlatform.name = "PsVita";
	psvitaPlatform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_PSVITA];
	psvitaPlatform.isSupported = true;
	psvitaPlatform.supportBuildAndRun = false;
	psvitaPlatform.supportBuildAndRunOnHardware = false;
	psvitaPlatform.platform = Platform::P_PsVita;
	psvitaPlatform.settings = std::make_shared<PlatformSettingsPsVita>(onSettingChangedEvent);

	/*BuildPlatform ps2Platform = BuildPlatform();
	ps2Platform.name = "PS2";
	ps2Platform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_PS2];
	ps2Platform.isSupported = false;
	ps2Platform.supportBuildAndRun = false;
	ps2Platform.supportBuildAndRunOnHardware = false;
	ps2Platform.platform = Platform::P_PS2;*/

	BuildPlatform ps3Platform = BuildPlatform();
	ps3Platform.name = "PS3";
	ps3Platform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_PS3];
	ps3Platform.isSupported = true;
	ps3Platform.supportBuildAndRun = false;
	ps3Platform.supportBuildAndRunOnHardware = false;
	ps3Platform.platform = Platform::P_PS3;
	ps3Platform.settings = std::make_shared<PlatformSettingsPS3>(onSettingChangedEvent);

	/*BuildPlatform ps4Platform = BuildPlatform();
	ps4Platform.name = "PS4";
	ps4Platform.icon = EditorIcons::GetIcons()[(int)IconName::Icon_Platform_PS4];
	ps4Platform.isSupported = false;
	ps4Platform.supportBuildAndRun = false;
	ps4Platform.supportBuildAndRunOnHardware = false;
	ps4Platform.platform = Platform::P_PS4;*/

	buildPlatforms.push_back(windowsPlatform);
	buildPlatforms.push_back(pspPlatform);
	buildPlatforms.push_back(psvitaPlatform);
	buildPlatforms.push_back(ps3Platform);
	buildPlatforms.push_back(linuxPlatform);
	//buildPlatforms.push_back(ps2Platform);
	//buildPlatforms.push_back(ps4Platform);

	onSettingChangedEvent->Bind(&BuildSettingsMenu::OnSettingChanged, this);
}

void BuildSettingsMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(999999, 999999));

	const bool visible = ImGui::Begin("Build Settings", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		bool isGameStopped = GameplayManager::GetGameState() == GameState::Stopped;
		ImGuiStyle& style = ImGui::GetStyle();
		// Create table
		if (ImGui::BeginTable("build_settings_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			// Create two columns
			ImGui::TableSetupColumn(0, ImGuiTableColumnFlags_WidthFixed,
				260.0f);
			ImGui::TableSetupColumn(0, ImGuiTableColumnFlags_WidthStretch,
				0);

			ImGui::TableNextRow();

			// Platforms list
			ImGui::TableSetColumnIndex(0);
			ImGui::BeginChild("build_settings_platforms_table_child");
			//ImGui::Text("Platforms");
			const int imageSize = 50;

			const size_t platformCount = buildPlatforms.size();
			ImVec2 availColSize = ImGui::GetContentRegionAvail();
			for (size_t i = 0; i < platformCount; i++)
			{
				const BuildPlatform& platform = buildPlatforms[i];
				ImVec2 cursorPos = ImGui::GetCursorPos();
				const ImVec2 startcursorPos = cursorPos;
				const float scrollY = ImGui::GetScrollY();
				ImGui::BeginGroup();

				// Change color/text if supported or not supported
				ImVec4 tint = ImVec4(1, 1, 1, 1);
				ImVec4 textColor = ImVec4(1, 1, 1, 1);
				std::string nameText = platform.name;
				Vector4 backgroundRGBA = EngineSettings::values.secondaryColor.GetRGBA().ToVector4();
				float backgroundColorCoef = 0.3f;
				if (!platform.isSupported)
				{
					tint = ImVec4(0.5f, 0.5f, 0.5f, 1);
					textColor = ImVec4(0.5f, 0.5f, 0.5f, 1);
					nameText += " (not yet supported)";
					backgroundColorCoef = 0.45f;
				}
				else
				{
					if (i == selectedPlatformIndex)
						backgroundColorCoef = 0.17f;
					else
						backgroundColorCoef = 0.35f;
				}
				backgroundRGBA.x = std::max(0.0f, backgroundRGBA.x - backgroundColorCoef);
				backgroundRGBA.y = std::max(0.0f, backgroundRGBA.y - backgroundColorCoef);
				backgroundRGBA.z = std::max(0.0f, backgroundRGBA.z - backgroundColorCoef);
				const ImU32 backgroundColor = IM_COL32(255 * backgroundRGBA.x, 255 * backgroundRGBA.y, 255 * backgroundRGBA.z, 200);

				// Draw button background
				const ImVec2 winPos = ImGui::GetWindowPos();
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(winPos.x + cursorPos.x, winPos.y + cursorPos.y - scrollY),
					ImVec2(winPos.x + cursorPos.x + availColSize.x, winPos.y + cursorPos.y - scrollY + 50 + 10),
					backgroundColor, 5);

				// Draw icon
				cursorPos.y += 5;
				ImGui::SetCursorPosX(cursorPos.x + 5);
				ImGui::SetCursorPosY(cursorPos.y);
				ImGui::Image((ImTextureID)(size_t)EditorUI::GetTextureId(*platform.icon), ImVec2(imageSize, imageSize), ImVec2(0, 0), ImVec2(1, 1), tint);

				// Draw icon shadow
				ImGui::SetCursorPosX(cursorPos.x + 5 + 2);
				ImGui::SetCursorPosY(cursorPos.y + 2);
				ImGui::Image((ImTextureID)(size_t)EditorUI::GetTextureId(*platform.icon), ImVec2(imageSize, imageSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0.2f));

				// Draw text
				ImGui::SameLine();
				ImGui::SetCursorPosY(cursorPos.y + 25 - style.ItemSpacing.y * 2);
				ImGui::TextColored(textColor, "%s", nameText.c_str());

				if (platform.isSupported)
				{
					ImGui::SetCursorPos(startcursorPos);
					if (ImGui::InvisibleButton(EditorUI::GenerateItemId().c_str(), ImVec2(availColSize.x, 50 + 10)))
					{
						selectedPlatformIndex = i;
						lastSettingError = 0;
					}
				}
				ImGui::EndGroup();

				if (i != platformCount - 1)
				{
					ImGui::SetCursorPosY(startcursorPos.y + 50 + 16);
				}
			}

			ImGui::EndChild();

			// Settings list
			ImGui::TableSetColumnIndex(1);
			ImGui::BeginChild("build_settings_settings_table_child");
			//ImGui::Text("Settings");
			const BuildPlatform& platform = buildPlatforms[selectedPlatformIndex];

			ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(*platform.settings, Application::PlatformToAssetPlatform(platform.platform));
			const bool valueChanged = EditorUI::DrawReflectiveData(reflectiveDataToDraw, platform.settings->GetReflectiveData(), onSettingChangedEvent) != ValueInputState::NO_CHANGE;
			if (valueChanged && reflectiveDataToDraw.command)
			{
				CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
				platform.settings->OnReflectionUpdated();
			}

			if (platform.platform == Platform::P_Windows)
			{
				ImGui::Text("Notes:");
				ImGui::Text("Icon: ico 512x512");
			}
			else if (platform.platform == Platform::P_PSP)
			{
				ImGui::Text("Notes:");
				ImGui::Text("Background image: PNG 480x272");
				ImGui::Text("Icon image: PNG 144x80");
				ImGui::Text("Preview image: PNG 310x180");
				if (lastSettingError == static_cast<int>(PlatformSettingsErrorPSP::WrongBackgroundSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong background image size");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPSP::WrongIconSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong icon image size");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPSP::WrongPreviewSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong preview image size");
				}
			}
			else if (platform.platform == Platform::P_PsVita)
			{
				ImGui::Text("Notes:");
				ImGui::Text("Background image: 8bits PNG 840x500");
				ImGui::Text("Icon image: 8bits PNG 128x128");
				ImGui::Text("Startup image: 8bits PNG 280x158");
				ImGui::Text("Game Id: Must be exactly 9 characters and unique");
				ImGui::TextWrapped("Recommended: XXXXYYYYY where X = string of developer in uppercase and Y = a number for this app");
				if (lastSettingError == static_cast<int>(PlatformSettingsErrorPsVita::WrongBackgroundSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong background image size");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPsVita::WrongIconSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong icon image size");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPsVita::WrongStartupImageSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Wrong startup image size");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPsVita::WrongGameIdSize))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Incorrect game id length");
				}
				else if (lastSettingError == static_cast<int>(PlatformSettingsErrorPsVita::WrongGameId))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Incorrect character found in game id");
				}
			}

			availColSize = ImGui::GetContentRegionAvail();
			if (platform.supportBuildAndRunOnHardware)
			{
				ImGui::SetCursorPosY(windowSize.y - (20 + style.ItemSpacing.y) * 2 * EditorUI::GetUiScale());
				ImGui::SetCursorPosX(availColSize.x - (180 + style.ItemSpacing.x) * EditorUI::GetUiScale());
				if (ImGui::Button("Build And Run On Hardware", ImVec2((180 + style.ItemSpacing.x) * EditorUI::GetUiScale(), 20 * EditorUI::GetUiScale())))
				{
					StartBuild(platform, BuildType::BuildAndRunOnHardwareGame);
				}
			}

			if (platform.supportBuildAndRun)
				ImGui::SetCursorPosX(availColSize.x - (180 + style.ItemSpacing.x) * EditorUI::GetUiScale());
			else
				ImGui::SetCursorPosX(availColSize.x - (80) * EditorUI::GetUiScale());
			ImGui::SetCursorPosY(windowSize.y - (20 + style.ItemSpacing.y) * EditorUI::GetUiScale());
			ImGui::BeginDisabled(!isGameStopped);
			if (ImGui::Button("Build", ImVec2(80 * EditorUI::GetUiScale(), 20 * EditorUI::GetUiScale())))
			{
				StartBuild(platform, BuildType::BuildGame);
			}
			if (platform.supportBuildAndRun)
			{
				ImGui::SameLine();
				if (ImGui::Button("Build And Run", ImVec2(100 * EditorUI::GetUiScale(), 20 * EditorUI::GetUiScale())))
				{
					StartBuild(platform, BuildType::BuildAndRunGame);
				}
			}
			ImGui::EndDisabled();

			ImGui::EndChild();
		}
		ImGui::EndTable();
		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

const BuildPlatform& BuildSettingsMenu::GetBuildPlatform(Platform platform)
{
	for (const BuildPlatform& buildPlatform : buildPlatforms)
	{
		if (buildPlatform.platform == platform)
		{
			return buildPlatform;
		}
	}

	XASSERT(false, "[BuildSettingsMenu::GetBuildPlatform] No platform found");
	return buildPlatforms[0];
}

void BuildSettingsMenu::OnSettingChanged()
{
	SaveSettings();
}

void BuildSettingsMenu::OnOpen()
{
	LoadSettings();
}

void BuildSettingsMenu::LoadSettings()
{
	// Read file data
	std::shared_ptr<File> file = FileSystem::MakeFile(ProjectManager::GetProjectFolderPath() + "build_settings.json");
	if (!file->Open(FileMode::ReadOnly)) 
	{
		Debug::PrintError("[BuildSettingsMenu::LoadSettings] Error while opening build settings file: " + file->GetPath());
		return;
	}

	const std::string data = file->ReadAll();
	file->Close();

	if (!data.empty())
	{
		json buildSettingsData;
		try
		{
			// Parse data to json
			buildSettingsData = json::parse(data);
		}
		catch (const std::exception&)
		{
			Debug::PrintError("[BuildSettingsMenu::LoadSettings] Error while loading build settings");
			return;
		}

		// Use json to update settings values
		const size_t platformCount = buildPlatforms.size();
		for (size_t i = 0; i < platformCount; i++)
		{
			const BuildPlatform& plaform = buildPlatforms[i];
			if (plaform.settings)
			{
				ReflectionUtils::JsonToReflectiveData(buildSettingsData[plaform.name], plaform.settings->GetReflectiveData());
			}
		}
	}
}

void BuildSettingsMenu::SaveSettings()
{
	// Generate json from settings data
	const size_t platformCount = buildPlatforms.size();
	json buildSettingsData;

	for (size_t i = 0; i < platformCount; i++)
	{
		const BuildPlatform& plaform = buildPlatforms[i];
		if (plaform.settings)
			buildSettingsData[plaform.name]["Values"] = ReflectionUtils::ReflectiveDataToJson(plaform.settings->GetReflectiveData());
	}

	FileSystem::Delete(ProjectManager::GetProjectFolderPath() + "build_settings.json");

	// Write json into the file
	std::shared_ptr<File> file = FileSystem::MakeFile(ProjectManager::GetProjectFolderPath() + "build_settings.json");
	if (!file->Open(FileMode::WriteCreateFile))
	{
		Debug::PrintError("[BuildSettingsMenu::SaveSettings] Error while saving build settings: " + file->GetPath());
		return;
	}
	file->Write(buildSettingsData.dump(0));
	file->Close();
}

void BuildSettingsMenu::StartBuild(const BuildPlatform& buildPlatform, BuildType buildType)
{
	const int validityResult = buildPlatform.settings->IsValid();
	lastSettingError = validityResult;

	if (validityResult != 0)
		return;

	const std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
	if (!exportPath.empty())
	{
		if (buildPlatform.platform == Platform::P_PS3)
		{
			Compiler::CompileGameThreaded(buildPlatform, BuildType::BuildShadersAndGame, exportPath);
		}
		else
		{
			Compiler::CompileGameThreaded(buildPlatform, buildType, exportPath);
		}
	}
}
