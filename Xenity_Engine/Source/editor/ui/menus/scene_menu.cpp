#include "scene_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"

void SceneMenu::Init()
{
}

void SceneMenu::Draw()
{
	int cameraCount = Graphics::cameras.size();
	std::weak_ptr<Camera> camera;
	Vector2Int frameBufferSize;
	for (int i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].lock()->isEditor)
		{
			camera = Graphics::cameras[i];
			frameBufferSize = camera.lock()->framebufferSize;
			break;
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string windowName = "Scene " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y) + "###Scene";
	ImGui::Begin(windowName.c_str());
	ImVec2 size = ImGui::GetContentRegionAvail();
	if (camera.lock())
	{
		camera.lock()->ChangeFrameBufferSize(Vector2Int(size.x, size.y));
	}
	if (camera.lock())
	{
		ImGui::Image((ImTextureID)camera.lock()->framebufferTexture, size, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
