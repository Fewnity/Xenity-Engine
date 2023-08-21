#include "select_project_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../../engine/asset_management/project_manager.h"

void SelectProjectMenu::Init()
{
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

	}
	ImGui::SameLine();
	if (ImGui::Button("Load project"))
	{
		std::string projectPath = EditorUI::OpenFolderDialog("Select project folder");
		if (projectPath != "")
		{
			ProjectManager::LoadProject(projectPath);
		}
	}
	ImGui::PopFont();

	//Reset font
	font->Scale = oldScale;
	ImGui::PushFont(font);
	ImGui::PopFont();

	ImGui::End();
}
