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
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Lighting", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		bool changed = false;
		ImGui::Text("Lighting");
		ImGui::Separator();
		EditorUI::DrawInput("Skybox", Graphics::skybox);

		//std::shared_ptr<int> t;
		//std::reference_wrapper<std::shared_ptr<int>> ref = std::ref(t);
		//EditorUI::DrawFileReference(&ref, "r", t);

		if (EditorUI::DrawInput("Color", Graphics::skyColor))
			changed = true;

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Fog");
		ImGui::Separator();

		if (EditorUI::DrawInputTemplate("Enabled", Graphics::isFogEnabled) != ValueInputState::NO_CHANGE)
			changed = true;
		if (EditorUI::DrawInputTemplate("Start", Graphics::fogStart) != ValueInputState::NO_CHANGE)
			changed = true;
		if (EditorUI::DrawInputTemplate("End", Graphics::fogEnd) != ValueInputState::NO_CHANGE)
			changed = true;
		if (EditorUI::DrawInput("Color", Graphics::fogColor))
			changed = true;

		if (changed)
		{
			Graphics::OnLightingSettingsReflectionUpdate();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
