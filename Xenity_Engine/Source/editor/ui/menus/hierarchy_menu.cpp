#include "hierarchy_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse);

	ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);
	bool itemClicked = false;
	//Add in the list only gameobject without parent
	for (int i = 0; i < Engine::gameObjectCount; i++)
	{
		if (Engine::gameObjects[i]->parent.lock() == nullptr)
		{
			if (EditorUI::DrawTreeItem(Engine::gameObjects[i])) 
			{
				itemClicked = true;
			}
		}
	}
	isFocused = ImGui::IsWindowFocused();
	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
	{
		Engine::selectedGameObject.reset();
		Engine::SetSelectedFileReference(nullptr);
	}
	ImGui::EndChild();

	ImGui::End();
}
