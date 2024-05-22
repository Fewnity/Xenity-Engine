// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "select_project_menu.h"
#include <imgui/imgui.h>

#include <engine/asset_management/project_manager.h>
#include <editor/editor.h>
#include <editor/ui/editor_ui.h>
#include <engine/debug/debug.h>

SelectProjectMenu::SelectProjectMenu()
{
	group = MenuGroup::Menu_Select_Project;
}

void SelectProjectMenu::Init()
{
	projectsList = ProjectManager::GetProjectsList();
}

void SelectProjectMenu::Draw()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	const bool visible = ImGui::Begin("Select Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	if (visible)
	{
		OnStartDrawing();

		//Increase font size
		ImFont* font = ImGui::GetFont();
		const float oldScale = font->Scale;
		font->Scale *= 2;
		ImGui::PushFont(font);

		//Draw text
		const std::string noCamText = "Projects";
		const ImVec2 textSize = ImGui::CalcTextSize(noCamText.c_str());

		ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
		ImGui::Text(noCamText.c_str());

		ImGui::PopFont();

		font->Scale = oldScale * 1.5f;
		ImGui::PushFont(font);
		if (ImGui::Button("Create project"))
		{
			Editor::currentMenu = MenuGroup::Menu_Create_Project;
		}
		ImGui::SameLine();
		if (ImGui::Button("Load project"))
		{
			OnLoadButtonClick();
		}

		DrawProjectsList();

		ImGui::PopFont();

		//Reset font
		font->Scale = oldScale;
		ImGui::PushFont(font);
		ImGui::PopFont();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void SelectProjectMenu::OnLoadButtonClick()
{
	const std::string projectPath = EditorUI::OpenFolderDialog("Select project folder", "");
	if (!projectPath.empty())
	{
		if (ProjectManager::LoadProject(projectPath))
		{
			// Check if the project is already in the opened projects list
			bool projectAlreadyInList = false;
			const size_t projectsCount = projectsList.size();
			for (size_t i = 0; i < projectsCount; i++)
			{
				if (projectsList[i].path == projectPath)
				{
					projectAlreadyInList = true;
					break;
				}
			}

			// If not, add the project to the list
			if (!projectAlreadyInList)
			{
				// Create new item
				ProjectListItem newProjectListItem;
				newProjectListItem.name = ProjectManager::GetProjectName();
				newProjectListItem.path = projectPath;
				projectsList.push_back(newProjectListItem);

				ProjectManager::SaveProjectsList(projectsList);
			}

			Editor::currentMenu = MenuGroup::Menu_Editor;
		}
		else
		{
			Debug::PrintError("[SelectProjectMenu::OnLoadButtonClick] This is not a Xenity Project", true);
		}
	}
}

void SelectProjectMenu::DrawProjectsList()
{
	ImGui::Separator();
	const size_t projectCount = projectsList.size();
	for (size_t i = 0; i < projectCount; i++)
	{
		ImGui::BeginGroup();
		ImGui::Text(projectsList[i].name.c_str());
		ImGui::Text(projectsList[i].path.c_str());
		ImGui::EndGroup();

		if (ImGui::IsItemClicked(0))
		{
			if (ProjectManager::LoadProject(projectsList[i].path))
			{
				Editor::currentMenu = MenuGroup::Menu_Editor;
			}
			else
			{
				Debug::PrintError("[SelectProjectMenu::DrawProjectsList] This is not a Xenity Project", true);
			}
		}
		ImGui::Separator();
	}
}
