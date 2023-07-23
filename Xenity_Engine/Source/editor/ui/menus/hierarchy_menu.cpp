#include "hierarchy_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSizeConstraints(ImVec2(50, viewport->Size.y - 20), ImVec2(viewport->Size.x / 2.0f, viewport->Size.y - 20));

	ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
	//ImGui::SetWindowFontScale(2);
	//if (!ImGui::IsWindowCollapsed())
	//{
	ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

	//Add in the list only gameobject without parent
	for (int i = 0; i < Engine::gameObjectCount; i++)
	{
		if (Engine::gameObjects[i]->parent.lock() == nullptr)
		{
			EditorUI::DrawTreeItem(Engine::gameObjects[i]);
		}
	}
	ImGui::EndChild();
	//}

	ImGui::End();
}
