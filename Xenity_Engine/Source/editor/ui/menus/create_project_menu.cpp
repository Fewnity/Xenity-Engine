#include "create_project_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/file_system/directory.h>
#include <engine/asset_management/project_manager.h>

void CreateProjectMenu::Init()
{
}

void  CreateProjectMenu::DrawTitle()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	const char* noCamText = "Create a project";
	ImVec2 textSize = ImGui::CalcTextSize(noCamText);
	ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
	ImGui::Text("%s", noCamText);
}

void  CreateProjectMenu::DrawProjectPath()
{
	// Draw project path
	std::string projectFolderText = "Project folder: " + projectParentDir + projectName + "\\";
	ImGui::Text("%s", projectFolderText.c_str());
}

bool CreateProjectMenu::DrawSelectFolderButton()
{
	// Draw select folder button
	bool projectFolderChanged = false;
	if (ImGui::Button("Select a folder"))
	{
		std::string folder = EditorUI::OpenFolderDialog("Select a folder");
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
	return EditorUI::DrawInput("Project Name", projectName);
}

void CreateProjectMenu::DrawError() 
{
	ImColor red = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
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
			std::shared_ptr <Directory> projectDir = std::make_shared<Directory>(projectParentDir + projectName);
			if (projectDir->CheckIfExist())
			{
				createProjectError = CreateProjectError::ERROR_PROJECT_ALREADY_EXISTS;
			}
			else
			{
				bool creationResult = ProjectManager::CreateProject(projectName, projectParentDir);
				if (creationResult)
				{
					std::vector<ProjectListItem> projectsList = ProjectManager::GetProjectsList();
					ProjectListItem newProjectListItem;
					newProjectListItem.name = projectName;
					newProjectListItem.path = projectParentDir + projectName + "\\";
					projectsList.push_back(newProjectListItem);
					ProjectManager::SaveProjectsList(projectsList);
					Editor::currentMenu = Menu_Editor;
				}
			}
			//delete projectDir;
		}
	}
}

void CreateProjectMenu::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	bool visible = ImGui::Begin("Create Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	if (visible)
	{
		//Increase font size to 150%
		ImFont* font = ImGui::GetFont();
		float oldScale = font->Scale;
		font->Scale *= 1.5f;
		ImGui::PushFont(font);

		if (ImGui::Button("Back"))
		{
			Editor::currentMenu = Menu_Select_Project;
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
	}
	ImGui::End();
}
