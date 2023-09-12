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
	ImGui::Begin("Lighting", &EditorUI::showLightingSettings, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Lighting");
	EditorUI::DrawInput("Skybox", Graphics::skybox);
	ImGui::Text("Fog");
	
	bool changed = false;
	if (EditorUI::DrawInput("Enabled", Graphics::isFogEnabled))
		changed = true;
	if (EditorUI::DrawInput("Start", Graphics::fogStart))
		changed = true;
	if (EditorUI::DrawInput("End", Graphics::fogEnd))
		changed = true;
	if (EditorUI::DrawInput("Color", Graphics::fogColor))
		changed = true;

	if (changed) 
	{
		Graphics::OnLightingSettingsReflectionUpdate();
	}

	ImGui::End();
}
