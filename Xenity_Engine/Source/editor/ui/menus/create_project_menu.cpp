// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "create_project_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/directory.h>
#include <engine/asset_management/project_manager.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <ShlObj.h>
#endif

CreateProjectMenu::CreateProjectMenu()
{
	group = MenuGroup::Menu_Create_Project;
#if defined(_WIN32) || defined(_WIN64)
	// Get Xenity's default project location
	TCHAR docPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_MYDOCUMENTS,
		NULL,
		0,
		docPath))) 
	{
		std::wstring wStringDocPath = &docPath[0]; //convert to wstring
		std::string stringDocPath(wStringDocPath.begin(), wStringDocPath.end()); //and convert to string.
		projectParentDir = stringDocPath + "\\Xenity_Projects\\";
		// Create the directory if not found
		const bool folderCreateResult = FileSystem::fileSystem->CreateFolder(projectParentDir);
		if (!folderCreateResult) 
		{
			projectParentDir.clear();
		}
	}
	#endif
}

void CreateProjectMenu::Init()
{
}

void  CreateProjectMenu::DrawTitle()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const char* noCamText = "Create a project";
	const ImVec2 textSize = ImGui::CalcTextSize(noCamText);
	ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
	ImGui::Text("%s", noCamText);
}

void  CreateProjectMenu::DrawProjectPath()
{
	// Draw project path
	const std::string projectFolderText = "Project folder: " + projectParentDir + projectName + "\\";
	ImGui::Text("%s", projectFolderText.c_str());
}

bool CreateProjectMenu::DrawSelectFolderButton()
{
	// Draw select folder button
	bool projectFolderChanged = false;
	if (ImGui::Button("Select a folder"))
	{
		const std::string folder = EditorUI::OpenFolderDialog("Select a folder", "");
		if (!folder.empty())
		{
			projectParentDir = folder;
			projectFolderChanged = true;
		}
	}
	return projectFolderChanged;
}

bool CreateProjectMenu::DrawProjectNameInput()
{
	return EditorUI::DrawInputTemplate("Project Name", projectName) != ValueInputState::NO_CHANGE;
}

void CreateProjectMenu::DrawError() 
{
	const ImColor red = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
	if (createProjectError == CreateProjectError::ERROR_PROJECT_ALREADY_EXISTS)
	{
		ImGui::TextColored(red, "A project has already this name");
	}
	else if (createProjectError == CreateProjectError::ERROR_EMPTY_FOLDER)
	{
		ImGui::TextColored(red, "Project folder not selected");
	}
	else if (createProjectError == CreateProjectError::ERROR_EMPTY_NAME)
	{
		ImGui::TextColored(red, "Project name empty");
	}
}

void CreateProjectMenu::DrawCreateProjectButton()
{
	// Draw create project button
	if (ImGui::Button("Create project"))
	{
		if (projectParentDir.empty())
		{
			createProjectError = CreateProjectError::ERROR_EMPTY_FOLDER;
		}
		else if (projectName.empty())
		{
			createProjectError = CreateProjectError::ERROR_EMPTY_NAME;
		}
		else
		{
			const std::shared_ptr <Directory> projectDir = std::make_shared<Directory>(projectParentDir + projectName);
			if (projectDir->CheckIfExist())
			{
				createProjectError = CreateProjectError::ERROR_PROJECT_ALREADY_EXISTS;
			}
			else
			{
				const bool creationResult = ProjectManager::CreateProject(projectName, projectParentDir);
				if (creationResult)
				{
					std::vector<ProjectListItem> projectsList = ProjectManager::GetProjectsList();
					ProjectListItem newProjectListItem;
					newProjectListItem.name = projectName;
					newProjectListItem.path = projectParentDir + projectName + "\\";
					projectsList.push_back(newProjectListItem);
					ProjectManager::SaveProjectsList(projectsList);
					Editor::currentMenu = MenuGroup::Menu_Editor;
				}
			}
		}
	}
}

void CreateProjectMenu::Draw()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	const bool visible = ImGui::Begin("Create Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	if (visible)
	{
		OnStartDrawing();

		//Increase font size to 150%
		ImFont* font = ImGui::GetFont();
		const float oldScale = font->Scale;
		font->Scale *= 1.5f;
		ImGui::PushFont(font);

		if (ImGui::Button("Back"))
		{
			Editor::currentMenu = MenuGroup::Menu_Select_Project;
		}

		// Set text scale to 200%
		ImGui::PopFont();
		font->Scale = oldScale * 2.0f;
		ImGui::PushFont(font);

		DrawTitle();


		// Set text scale to 150%
		ImGui::PopFont();
		font->Scale = oldScale * 1.5f;
		ImGui::PushFont(font);

		DrawProjectPath();

		bool projectFolderChanged = DrawSelectFolderButton();

		bool nameChanged = DrawProjectNameInput();

		if (projectFolderChanged && createProjectError == CreateProjectError::ERROR_EMPTY_FOLDER)
		{
			createProjectError = CreateProjectError::NO_ERROR_;
		}
		else if ((nameChanged || projectFolderChanged) && createProjectError == CreateProjectError::ERROR_PROJECT_ALREADY_EXISTS)
		{
			createProjectError = CreateProjectError::NO_ERROR_;
		}

		DrawError();

		DrawCreateProjectButton();

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
