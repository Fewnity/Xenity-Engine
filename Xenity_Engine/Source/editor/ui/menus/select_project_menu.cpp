#include "select_project_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"
#include "../editor_ui.h"
#include "../../../engine/asset_management/project_manager.h"

std::vector<ProjectListItem> projectsList;

void SelectProjectMenu::Init()
{
	projectsList = ProjectManager::GetProjectsList();
}

void SelectProjectMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("Select Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImVec2 size = ImGui::GetContentRegionAvail();

	//Increase font size
	ImFont* font = ImGui::GetFont();
	float oldScale = font->Scale;
	font->Scale *= 2;
	ImGui::PushFont(font);

	//Draw text
	std::string noCamText = "Projects";
	ImVec2 textSize = ImGui::CalcTextSize(noCamText.c_str());
	float offY = ImGui::GetCursorPosY();
	ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
	ImGui::Text(noCamText.c_str());

	ImGui::PopFont();

	font->Scale = oldScale * 1.5f;
	ImGui::PushFont(font);
	if (ImGui::Button("Create project"))
	{
		Editor::currentMenu = Menu_Create_Project;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load project"))
	{
		std::string projectPath = EditorUI::OpenFolderDialog("Select project folder");
		if (projectPath != "")
		{
			if (ProjectManager::LoadProject(projectPath))
			{
				// Check if the project is already in the opened projects list
				std::vector<ProjectListItem> projectsList = ProjectManager::GetProjectsList();
				bool projectAlreadyInList = false;
				int projectsCount = projectsList.size();
				for (int i = 0; i < projectsCount; i++)
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

				Editor::currentMenu = Menu_Editor;
			}
			else
			{
				Debug::PrintError("This is not a Xenity Project");
			}
		}
	}
	ImGui::Separator();
	int projectCount = projectsList.size();
	for (int i = 0; i < projectCount; i++)
	{
		ImGui::BeginGroup();
		ImGui::Text(projectsList[i].name.c_str());
		ImGui::Text(projectsList[i].path.c_str());
		ImGui::EndGroup();

		if (ImGui::IsItemClicked(0))
		{
			if (ProjectManager::LoadProject(projectsList[i].path))
			{
				Editor::currentMenu = Menu_Editor;
			}
			else
			{
				Debug::PrintError("This is not a Xenity Project");
			}
		}
		ImGui::Separator();
	}

	ImGui::PopFont();

	//Reset font
	font->Scale = oldScale;
	ImGui::PushFont(font);
	ImGui::PopFont();

	ImGui::End();
}
