#include "build_settings_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <editor/compiler.h>
#include <engine/engine_settings.h>

void BuildSettingsMenu::Init()
{
	BuildPlatform windowsPlatform = BuildPlatform();
	windowsPlatform.name = "Windows";
	windowsPlatform.icon = EditorUI::icons[(int)IconName::Icon_Platform_Windows];
	windowsPlatform.isSupported = true;
	windowsPlatform.supportBuildAndRun = true;
	windowsPlatform.supportBuildAndRunOnHardware = false;
	windowsPlatform.platform = Platform::P_Windows;
	windowsPlatform.settings = std::make_shared<PlatformSettingsWindows>();

	BuildPlatform pspPlatform = BuildPlatform();
	pspPlatform.name = "PSP";
	pspPlatform.icon = EditorUI::icons[(int)IconName::Icon_Platform_PSP];
	pspPlatform.isSupported = true;
	pspPlatform.supportBuildAndRun = true;
	pspPlatform.supportBuildAndRunOnHardware = true;
	pspPlatform.platform = Platform::P_PSP;
	pspPlatform.settings = std::make_shared<PlatformSettingsPSP>();

	BuildPlatform psvitaPlatform = BuildPlatform();
	psvitaPlatform.name = "PsVita";
	psvitaPlatform.icon = EditorUI::icons[(int)IconName::Icon_Platform_PSVITA];
	psvitaPlatform.isSupported = true;
	psvitaPlatform.supportBuildAndRun = false;
	psvitaPlatform.supportBuildAndRunOnHardware = false;
	psvitaPlatform.platform = Platform::P_PsVita;
	psvitaPlatform.settings = std::make_shared<PlatformSettingsPsVita>();

	BuildPlatform ps2Platform = BuildPlatform();
	ps2Platform.name = "PS2";
	ps2Platform.icon = EditorUI::icons[(int)IconName::Icon_Platform_PS2];
	ps2Platform.isSupported = false;
	ps2Platform.supportBuildAndRun = false;
	ps2Platform.supportBuildAndRunOnHardware = false;
	ps2Platform.platform = Platform::P_PS2;

	BuildPlatform ps3Platform = BuildPlatform();
	ps3Platform.name = "PS3";
	ps3Platform.icon = EditorUI::icons[(int)IconName::Icon_Platform_PS3];
	ps3Platform.isSupported = false;
	ps3Platform.supportBuildAndRun = false;
	ps3Platform.supportBuildAndRunOnHardware = false;
	ps3Platform.platform = Platform::P_PS3;

	BuildPlatform ps4Platform = BuildPlatform();
	ps4Platform.name = "PS4";
	ps4Platform.icon = EditorUI::icons[(int)IconName::Icon_Platform_PS4];
	ps4Platform.isSupported = false;
	ps4Platform.supportBuildAndRun = false;
	ps4Platform.supportBuildAndRunOnHardware = false;
	ps4Platform.platform = Platform::P_PS4;

	plaforms.push_back(windowsPlatform);
	plaforms.push_back(pspPlatform);
	plaforms.push_back(psvitaPlatform);
	plaforms.push_back(ps2Platform);
	plaforms.push_back(ps3Platform);
	plaforms.push_back(ps4Platform);
}

void BuildSettingsMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Build Settings", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		if (ImGui::BeginTable("build_settings_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn(0, ImGuiTableColumnFlags_WidthFixed,
				260.0f);
			ImGui::TableSetupColumn(0, ImGuiTableColumnFlags_WidthStretch,
				0);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::BeginChild("build_settings_platforms_table_child");
			//ImGui::Text("Platforms");
			const int imageSize = 50;

			const int platformCount = plaforms.size();
			for (int i = 0; i < platformCount; i++)
			{
				const BuildPlatform& platform = plaforms[i];
				ImVec2 cursorPos = ImGui::GetCursorPos();
				const ImVec2 startcursorPos = cursorPos;
				const float scrollY = ImGui::GetScrollY();
				const ImVec2 availColSize = ImGui::GetContentRegionAvail();
				ImGui::BeginGroup();
				ImVec4 tint = ImVec4(1, 1, 1, 1);
				ImVec4 textColor = ImVec4(1, 1, 1, 1);
				std::string nameText = platform.name;
				Vector4 backgroundRGBA = EngineSettings::secondaryColor.GetRGBA().ToVector4();
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

				const ImVec2 winPos = ImGui::GetWindowPos();
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(winPos.x + cursorPos.x, winPos.y + cursorPos.y - scrollY),
					ImVec2(winPos.x + cursorPos.x + availColSize.x, winPos.y + cursorPos.y - scrollY + 50 + 10),
					backgroundColor, 5);

				cursorPos.y += 5;
				ImGui::SetCursorPosX(cursorPos.x + 5);
				ImGui::SetCursorPosY(cursorPos.y);
				ImGui::Image((ImTextureID)platform.icon->GetTextureId(), ImVec2(imageSize, imageSize), ImVec2(0,0), ImVec2(1, 1), tint);

				ImGui::SetCursorPosX(cursorPos.x + 5 + 2);
				ImGui::SetCursorPosY(cursorPos.y + 2);
				ImGui::Image((ImTextureID)platform.icon->GetTextureId(), ImVec2(imageSize, imageSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0.2f));

				ImGui::SameLine();
				ImGui::SetCursorPosY(cursorPos.y + 25 - ImGui::GetStyle().ItemSpacing.y * 2);
				ImGui::TextColored(textColor, "%s", nameText.c_str());

				if (platform.isSupported)
				{
					ImGui::SetCursorPos(startcursorPos);
					if (ImGui::InvisibleButton(EditorUI::GenerateItemId().c_str(), ImVec2(availColSize.x, 50 + 10)))
					{
						selectedPlatformIndex = i;
					}
				}
				ImGui::EndGroup();
				ImGui::SetCursorPosY(startcursorPos.y + 50 + 16);
			}

			ImGui::EndChild();

			ImGui::TableSetColumnIndex(1);
			ImGui::BeginChild("build_settings_settings_table_child");
			//ImGui::Text("Settings");
			const BuildPlatform& platform = plaforms[selectedPlatformIndex];
			std::shared_ptr<Command> command = nullptr;
			//std::shared_ptr<PlatformSettings> parent;
			bool valueChanged = EditorUI::DrawReflectiveData(platform.settings->GetReflectiveData(), command, platform.settings);
			if (valueChanged && command)
			{
				command->Execute();
			}
			if (ImGui::Button("Build"))
			{
				StartBuild(platform.platform, BuildType::BuildGame);
			}
			if (platform.supportBuildAndRun)
			{
				if (ImGui::Button("Build And Run"))
				{
					StartBuild(platform.platform, BuildType::BuildAndRunGame);
				}
			}
			if (platform.supportBuildAndRunOnHardware)
			{
				if (ImGui::Button("Build And Run On Hardware"))
				{
					StartBuild(platform.platform, BuildType::BuildAndRunOnHardwareGame);
				}
			}
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

void BuildSettingsMenu::LoadSettings()
{
}

void BuildSettingsMenu::SaveSettings()
{
}

void BuildSettingsMenu::StartBuild(Platform platform, BuildType buildType)
{
	const std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
	if (!exportPath.empty())
		Compiler::CompileGameThreaded(platform, BuildType::BuildGame, exportPath);
}
