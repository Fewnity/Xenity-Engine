#include "create_project_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"
#include "../editor_ui.h"
#include "../../../engine/asset_management/project_manager.h"

void CreateProjectMenu::Init()
{
}

void CreateProjectMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("Create Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImVec2 size = ImGui::GetContentRegionAvail();

	//Increase font size
	ImFont* font = ImGui::GetFont();
	float oldScale = font->Scale;
	font->Scale *= 2;
	ImGui::PushFont(font);

	//Draw text
	std::string noCamText = "Create a project";
	ImVec2 textSize = ImGui::CalcTextSize(noCamText.c_str());
	float offY = ImGui::GetCursorPosY();
	ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
	ImGui::Text(noCamText.c_str());

	ImGui::PopFont();

	font->Scale = oldScale * 1.5f;
	ImGui::PushFont(font);

	std::string projectFolderText = "Project folder: " + projectParentDir + projectName + "\\";
	ImGui::Text(projectFolderText.c_str());

	bool projectFolderChanged = false;
	if (ImGui::Button("Select a folder"))
	{
		std::string folder = EditorUI::OpenFolderDialog("Select a folder");
		if (folder != "")
		{
			projectParentDir = folder;
			projectFolderChanged = true;
		}
	}
	bool nameChanged = EditorUI::DrawInput("Project Name", projectName);

	if (projectFolderChanged && createProjectError == ERROR_EMPTY_FOLDER)
	{
		createProjectError = NO_ERROR;
	}
	else if ((nameChanged || projectFolderChanged) && createProjectError == ERROR_PROJECT_ALREADY_EXISTS)
	{
		createProjectError = NO_ERROR;
	}

	ImColor red = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
	if (createProjectError == ERROR_PROJECT_ALREADY_EXISTS)
	{
		ImGui::TextColored(red, "A project has already this name");
	}
	else if (createProjectError == ERROR_EMPTY_FOLDER)
	{
		ImGui::TextColored(red, "Project folder not selected");
	}
	else if (createProjectError == ERROR_EMPTY_NAME)
	{
		ImGui::TextColored(red, "Project name empty");
	}

	if (ImGui::Button("Create project"))
	{
		if (projectParentDir == "")
		{
			createProjectError = ERROR_EMPTY_FOLDER;
		}
		else if (projectName == "")
		{
			createProjectError = ERROR_EMPTY_NAME;
		}
		else
		{
			Directory* projectDir = new Directory(projectParentDir + projectName);
			if (projectDir->CheckIfExist())
			{
				createProjectError = ERROR_PROJECT_ALREADY_EXISTS;
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
		}
	}
	ImGui::PopFont();

	//Reset font
	font->Scale = oldScale;
	ImGui::PushFont(font);
	ImGui::PopFont();

	ImGui::End();
}
