#include "game_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"

void GameMenu::Init()
{
}

void GameMenu::Draw()
{
	windowSize = Vector2Int(0, 0);
	int cameraCount = Graphics::cameras.size();
	std::weak_ptr<Camera> camera;
	Vector2Int frameBufferSize;
	for (int i = 0; i < cameraCount; i++)
	{
		if (!Graphics::cameras[i].lock()->isEditor)
		{
			camera = Graphics::cameras[i];
			frameBufferSize = camera.lock()->framebufferSize;
			break;
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string windowName;
	if (camera.lock())
	{
		windowName = "Game " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y) + "###Game";
	}
	else
	{
		windowName = "Game (No camera)###Game";
	}
	ImGui::Begin(windowName.c_str());
	if (InputSystem::GetKeyDown(MOUSE_RIGHT))
	{
		ImGui::SetWindowFocus();
	}
	ImVec2 size = ImGui::GetContentRegionAvail();
	if(camera.lock())
	{
		camera.lock()->ChangeFrameBufferSize(Vector2Int(size.x, size.y));
		ImGui::Image((ImTextureID)camera.lock()->framebufferTexture, size, ImVec2(0, 1), ImVec2(1, 0));
		windowSize = Vector2Int(size.x, size.y);
		if (ImGui::IsItemHovered())
		{
			isHovered = true;
		}
		else 
		{
			isHovered = false;
		}
	}

	//windowSize = Vector2Int(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	windowPosition = Vector2Int(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	mousePosition = Vector2Int(ImGui::GetMousePos().x, ImGui::GetMousePos().y - (ImGui::GetWindowSize().y - size.y));

	ImGui::End();
	ImGui::PopStyleVar();
}
