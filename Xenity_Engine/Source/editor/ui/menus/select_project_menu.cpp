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
		ImGui::Text("%s", noCamText.c_str());

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
		ProjectLoadingErrors result = ProjectManager::LoadProject(projectPath);
		if (result == ProjectLoadingErrors::Success)
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
			ShowProjectError(result);
		}
	}
}

void SelectProjectMenu::DrawProjectsList()
{
	ImGui::Separator();

	size_t projectListSize = projectsList.size();
	for (size_t i = 0; i < projectListSize; i++)
	{
		ProjectListItem& project = projectsList[i];
		ImGui::BeginGroup();
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::Text("%s", project.name.c_str());
		ImGui::Text("%s", project.path.c_str());
		float availWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(availWidth - 50, cursorPos.y + 15));
		if (ImGui::Button((std::string("...") + EditorUI::GenerateItemId()).c_str()))
		{
			selectedProject = &project;
			ImGui::OpenPopup(std::to_string(*(size_t*)selectedProject).c_str());
		}
		if (selectedProject == &project)
		{

			if (ImGui::BeginPopup(std::to_string(*(size_t*)selectedProject).c_str()))
			{
				if (ImGui::MenuItem("Remove from list"))
				{
					DialogResult result = EditorUI::OpenDialog("Remove " + project.name, "Are you sure you want to remove the " + project.name + " project from the list?\n(Files won't be deleted)", DialogType::Dialog_Type_YES_NO_CANCEL);
					if (result == DialogResult::Dialog_YES)
					{
						DeleteProject(i, false);
						i--;
						projectListSize--;
					}
					selectedProject = nullptr;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Delete"))
				{
					DialogResult result = EditorUI::OpenDialog("Delete " + project.name, "Are you sure you want to delete the " + project.name + " project?\n(Files will be deleted)", DialogType::Dialog_Type_YES_NO_CANCEL);
					if (result == DialogResult::Dialog_YES)
					{
						DeleteProject(i, true);
						i--;
						projectListSize--;
					}
					selectedProject = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y));
		if (ImGui::InvisibleButton(EditorUI::GenerateItemId().c_str(), ImVec2(availWidth, 60)))
		{
			ProjectLoadingErrors result = ProjectManager::LoadProject(project.path);
			if (result == ProjectLoadingErrors::Success)
			{
				Editor::currentMenu = MenuGroup::Menu_Editor;
			}
			else
			{
				ShowProjectError(result);
			}
		}
		ImGui::EndGroup();

		ImGui::Separator();
	}
}

void SelectProjectMenu::DeleteProject(size_t projectIndex, bool deleteFiles)
{
	ProjectListItem& project = projectsList[projectIndex];
	if (deleteFiles && std::filesystem::exists(project.path))
	{
		try
		{
			std::filesystem::remove_all(project.path);
		}
		catch (const std::exception&)
		{
		}
	}
	projectsList.erase(projectsList.begin() + projectIndex);
	ProjectManager::SaveProjectsList(projectsList);
}

void SelectProjectMenu::ShowProjectError(ProjectLoadingErrors error)
{
	if (error == ProjectLoadingErrors::NoAssetFolder)
	{
		EditorUI::OpenDialog("Error", "This is not a Xenity Project, no asset folder found.", DialogType::Dialog_Type_OK);
		Debug::PrintError("[SelectProjectMenu::DrawProjectsList] This is not a Xenity Project", true);
	}
	else 
	{
		EditorUI::OpenDialog("Error", "Cannot open project.", DialogType::Dialog_Type_OK);
		Debug::PrintError("[SelectProjectMenu::ShowProjectError] Cannot open project", true);
	}
}
