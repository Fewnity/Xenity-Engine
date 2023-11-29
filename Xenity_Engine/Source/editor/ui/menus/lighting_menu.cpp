#include "lighting_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/graphics.h>

void LightingMenu::Init()
{
}

void LightingMenu::Draw()
{
	bool changed = false;
	bool visible = ImGui::Begin("Lighting", &EditorUI::showLightingSettings, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		ImGui::Text("Lighting");
		ImGui::Separator();
		EditorUI::DrawInput("Skybox", Graphics::skybox);

		if (EditorUI::DrawInput("Color", Graphics::skyColor))
			changed = true;

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Fog");
		ImGui::Separator();
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
	}
	ImGui::End();
}
