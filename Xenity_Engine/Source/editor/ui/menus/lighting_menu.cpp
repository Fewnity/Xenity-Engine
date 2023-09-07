#include "lighting_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"
#include "../../../engine/reflection/reflection.h"
#include "../../../engine/graphics/skybox.h"

void LightingMenu::Init()
{
}

void LightingMenu::Draw()
{
	ImGui::Begin("Lighting");
	ImGui::Text("Lighting");
	EditorUI::DrawInput("Skybox", Graphics::skybox);
	ImGui::End();
}
