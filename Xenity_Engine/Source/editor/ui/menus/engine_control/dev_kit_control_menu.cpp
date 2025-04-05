// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "dev_kit_control_menu.h"

#include <thread>
#include <imgui/imgui.h>

#include <editor/editor.h>

#include <engine/engine_settings.h>
#include <engine/tools/string_utils.h>

void DevKitControlMenu::Init()
{
}

void DevKitControlMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Dev Kit Control", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		devKitListMutex.lock();
		size_t devKitCount = devKits.size();
		devKitListMutex.unlock();
		if (ImGui::BeginTable("dev_kits_list", 1, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV))
		{
			ImGui::TableSetupColumn("Dev kits", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			devKitListMutex.lock();
			for (size_t i = 0; i < devKitCount; i++)
			{
				ImGui::TableNextRow(0, 0);
				ImGui::TableSetColumnIndex(0);
				std::string selectableText = devKits[i].name + " | ip: " + devKits[i].ip;
				if (i == selectedDevKit)
				{
					selectableText += " [selected]";
				}
				selectableText += "##" + std::to_string(i);
				if (ImGui::Selectable(selectableText.c_str(), i == selectedDevKit, 0))
				{
					selectedDevKit = static_cast<int>(i);
				}
			}
			devKitListMutex.unlock();

			ImGui::EndTable();
		}
		const bool canUpdateList = currentError != DevKitError::WaitingForResponse;
		if (!canUpdateList)
		{
			ImGui::BeginDisabled();
		}
		if (ImGui::Button("Update dev kits List"))
		{
			std::thread getListThread = std::thread(&DevKitControlMenu::GetDevKitList, this);
			getListThread.detach();
		}
		if (!canUpdateList)
		{
			ImGui::EndDisabled();
		}

		const bool validDevKitSelected = selectedDevKit != -1 && selectedDevKit < devKitCount && currentError != DevKitError::WaitingForResponse;

		ImGui::Separator();

		// Dev Kit power management
		ImGui::Text("Power Management");
		if (!validDevKitSelected)
		{
			ImGui::BeginDisabled();
		}
		if (ImGui::Button("Power On Dev Kit"))
		{
			std::thread powerOnThread = std::thread(&DevKitControlMenu::PowerOnDevKit, this);
			powerOnThread.detach();
		}
		ImGui::SameLine();
		if (ImGui::Button("Power Off Dev Kit"))
		{
			std::thread powerOffThread = std::thread(&DevKitControlMenu::PowerOffDevKit, this);
			powerOffThread.detach();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Dev Kit"))
		{
			std::thread resetThread = std::thread(&DevKitControlMenu::ResetDevKit, this);
			resetThread.detach();
		}
		if (!validDevKitSelected)
		{
			ImGui::EndDisabled();
		}
		ImGui::Separator();

		// Dev Kit game management
		ImGui::Text("Game Management");
		if (!validDevKitSelected)
		{
			ImGui::BeginDisabled();
		}
		if (ImGui::Button("Launch game from ps3 hdd"))
		{
			std::thread launchGameThread = std::thread(&DevKitControlMenu::LaunchGame, this, DevKitRunningMode::FromHDD);
			launchGameThread.detach();
		}
		if (ImGui::Button("Launch game from pc"))
		{
			std::thread launchGameThread = std::thread(&DevKitControlMenu::LaunchGame, this, DevKitRunningMode::FromPC);
			launchGameThread.detach();
		}
		if (!validDevKitSelected)
		{
			ImGui::EndDisabled();
		}
		ImGui::Separator();
		if (currentError == DevKitError::WaitingForResponse)
		{
			ImGui::Text("Waiting for response...");
		}
		else if (currentError != DevKitError::NoError)
		{
			std::string errorString = "Error: ";
			switch (currentError)
			{
			case DevKitError::FailedToPowerOn:
				errorString += "Failed to power on dev kit";
				break;
			case DevKitError::FailedToPowerOff:
				errorString += "Failed to power off dev kit";
				break;
			case DevKitError::FailedToReset:
				errorString += "Failed to reset dev kit";
				break;
			case DevKitError::FailedToGetList:
				errorString += "Failed to get dev kit list";
				break;
			case DevKitError::FailedToLaunchGame:
				errorString += "Failed to launch game";
				break;
			}
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorString.c_str());
		}
		//if (ImGui::Button("Listen for logs"))
		//{
		//	std::thread logThread = std::thread(&DevKitControlMenu::StartLogListening, this);
		//	logThread.detach();
		//}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void DevKitControlMenu::PowerOnDevKit()
{
	currentError = DevKitError::WaitingForResponse;

	std::string returnText = "";
	std::string command = "\"" + EngineSettings::values.ps3CtrlPath + "\"";
	command += " -t " + devKits[selectedDevKit].name;
	command += " power -on";
	const int result = Editor::ExecuteSystemCommand(command, returnText);
	if (result != 0)
	{
		currentError = DevKitError::FailedToPowerOn;
	}
	else
	{
		currentError = DevKitError::NoError;
	}
}

void DevKitControlMenu::PowerOffDevKit()
{
	currentError = DevKitError::WaitingForResponse;

	std::string returnText = "";
	std::string command = "\"" + EngineSettings::values.ps3CtrlPath + "\"";
	command += " -t " + devKits[selectedDevKit].name;
	command += " power -off";
	const int result = Editor::ExecuteSystemCommand(command, returnText);
	if (result != 0)
	{
		currentError = DevKitError::FailedToPowerOff;
	}
	else
	{
		currentError = DevKitError::NoError;
	}
}

void DevKitControlMenu::ResetDevKit()
{
	currentError = DevKitError::WaitingForResponse;

	std::string returnText = "";
	std::string command = "\"" + EngineSettings::values.ps3CtrlPath + "\"";
	command += " -t " + devKits[selectedDevKit].name;
	command += " run -r";

	const int result = Editor::ExecuteSystemCommand(command, returnText);
	if (result != 0)
	{
		currentError = DevKitError::FailedToReset;
	}
	else
	{
		currentError = DevKitError::NoError;
	}
}

void DevKitControlMenu::GetDevKitList()
{
	currentError = DevKitError::WaitingForResponse;

	std::string returnText = "";
	const int result = Editor::ExecuteSystemCommand("\"" + EngineSettings::values.ps3CtrlPath + "\" list", returnText);
	if (result != 0)
	{
		currentError = DevKitError::FailedToGetList;
	}
	else
	{
		currentError = DevKitError::NoError;
	}

	devKitListMutex.lock();
	devKits.clear();
	devKitListMutex.unlock();
	std::vector<std::string> stringList = StringUtils::Split(returnText, '-');
	for (size_t i = 0; i < stringList.size(); i++)
	{
		const size_t nameIndex = stringList[i].find("Name: ") + 6; // 6 is the length of "Name: "
		const size_t nameEndIndex = stringList[i].find("\n", nameIndex);
		const std::string name = stringList[i].substr(nameIndex, nameEndIndex - nameIndex);

		const size_t ipIndex = stringList[i].find("Host: ") + 6; // 6 is the length of "Host: "
		const size_t ipEndIndex = stringList[i].find("\n", ipIndex);
		const std::string ip = stringList[i].substr(ipIndex, ipEndIndex - ipIndex);

		devKitListMutex.lock();
		devKits.push_back({ name, ip });
		devKitListMutex.unlock();
	}
}

void DevKitControlMenu::LaunchGame(DevKitRunningMode devKitRunningMode)
{
	currentError = DevKitError::WaitingForResponse;

	std::string returnText = "";
	std::string command = "\"\"" + EngineSettings::values.ps3CtrlPath + "\"";
	// Option used to run the game from fake dev kit (retail console with DEX firmware) since fake dev kit cannot run games from PC
	command += " run -t " + devKits[selectedDevKit].name;
	if (devKitRunningMode == DevKitRunningMode::FromHDD)
	{
		command += " ../dev_hdd0/xenity_engine/XenityBuild.self";
	}
	else if (devKitRunningMode == DevKitRunningMode::FromPC)
	{
		command += " C:\\Users\\elect\\Desktop\\Builds\\Benchmark\\PS3\\XenityBuild.fake.self"; // Real dev kit cannot run normal self files so use fake.self
	}
	command += "\"";

	const int result = Editor::ExecuteSystemCommand(command, returnText);
	if (result != 0)
	{
		currentError = DevKitError::FailedToLaunchGame;
	}
	else
	{
		currentError = DevKitError::NoError;
	}
}

void DevKitControlMenu::StartLogListening()
{
	std::string returnText = "";
	std::string command = "\"" + EngineSettings::values.ps3CtrlPath + "\"";
	command += " -t " + devKits[selectedDevKit].name;
	command += " run -p"; // -p option is used to print logs and keep the process running

	const int result = Editor::ExecuteSystemCommand(command, returnText);
}
