#include "create_class_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"

void CreateClassMenu::Init()
{
}

void CreateClassMenu::Draw()
{
	ImGui::Begin("Create Class", &EditorUI::showCreateClass, ImGuiWindowFlags_NoCollapse);

	if (EditorUI::DrawInput("Class Name", className)) 
	{

	}

	if (ImGui::Button("Create") && !className.empty())
	{
		Editor::CreateNewFile(filePath + "\\" + className, FileType::File_Code);
		Editor::CreateNewFile(filePath + "\\" + className, FileType::File_Header);
		EditorUI::showCreateClass = false;
	}
	ImGui::End();
}
