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
	if (InputSystem::GetKeyDown(MOUSE_RIGHT) && ImGui::IsWindowHovered())
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
	else
	{
		//Increase font size
		ImFont* font = ImGui::GetFont();
		font->Scale *= 2;
		ImGui::PushFont(font);

		//Draw text
		std::string noCamText = "There is no camera";
		ImVec2 textSize = ImGui::CalcTextSize(noCamText.c_str());
		float offY = ImGui::GetCursorPosY();
		ImGui::SetCursorPos(ImVec2((size.x - textSize.x) / 2.0f, (size.y + offY) /2.0f));
		ImGui::Text(noCamText.c_str());
		ImGui::PopFont();

		//Reset font
		font->Scale /= 2.0f;
		ImGui::PushFont(font);
		ImGui::PopFont();
	}

	windowPosition = Vector2Int(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	mousePosition = Vector2Int(ImGui::GetMousePos().x, ImGui::GetMousePos().y - (ImGui::GetWindowSize().y - size.y));

	ImGui::End();
	ImGui::PopStyleVar();
}
