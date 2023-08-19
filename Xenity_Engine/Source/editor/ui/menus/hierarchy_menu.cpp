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
	//Add in the list only gameobject without parent
	for (int i = 0; i < Engine::gameObjectCount; i++)
	{
		if (Engine::gameObjects[i]->parent.lock() == nullptr)
		{
			EditorUI::DrawTreeItem(Engine::gameObjects[i]);
		}
	}

	isFocused = ImGui::IsWindowFocused();
	ImGui::EndChild();

	ImGui::End();
}
