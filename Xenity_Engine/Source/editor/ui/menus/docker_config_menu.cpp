// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "docker_config_menu.h"

#include <thread>
#include <imgui/imgui.h>

#include <editor/editor.h>

#include <engine/engine_settings.h>

void DockerConfigMenu::Init()
{
	dockerStateEvent.Bind(&DockerConfigMenu::SetDockerState, this);
	Refresh();
}

void DockerConfigMenu::Refresh() 
{
	std::thread refreshThread(&Compiler::CheckDockerState, &dockerStateEvent);
	refreshThread.detach();
}

void DockerConfigMenu::SetDockerState(const DockerState state)
{
	currentDockerState = state;
}

void DockerConfigMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Docker Config", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (ImGui::Button("Check Docker")) 
		{
			Refresh();
		}
		std::string stateText = "";
		ImVec4 color;
		if (currentDockerState == DockerState::NOT_INSTALLED) 
		{
			stateText = "Docker is not installed";
			color = ImVec4(1, 0, 0, 1);
		}
		else if (currentDockerState == DockerState::NOT_RUNNING)
		{
			stateText = "Docker is not running";
			color = ImVec4(1, 0.64f, 0, 1);
		}
		else if (currentDockerState == DockerState::MISSING_IMAGE)
		{
			stateText = "Docker image is missing";
			color = ImVec4(1, 0.64f, 0, 1);
		}
		else //if (currentDockerState == DockerState::RUNNING)
		{
			stateText = "Docker is running and setup is good";
			color = ImVec4(0, 1, 0, 1);
		}
		
		ImGui::TextColored(color, "%s", stateText.c_str());
		if (currentDockerState == DockerState::NOT_INSTALLED)
		{
			ImGui::Text("You have to install Docker on your computer");
			if (ImGui::Button("Docker website"))
			{
				Editor::OpenLinkInWebBrowser("https://www.docker.com/products/docker-desktop/");
			}
		}
		else if (currentDockerState == DockerState::MISSING_IMAGE)
		{
			ImGui::Text("You have to create the Ubuntu Docker image (automatic process, can take few minutes)");
			if (ImGui::Button("Create image"))
			{
				Compiler::CreateDockerImage();
				Refresh();
			}
		}
		else if (currentDockerState == DockerState::NOT_RUNNING)
		{
			ImGui::Text("You have to launch Docker");
			if (ImGui::Button("Start Docker"))
			{
				Editor::OpenExecutableFile(EngineSettings::values.dockerExePath);
			}
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
