#include "menu.h"
#include <imgui/imgui.h>

void Menu::Focus()
{
	forceFocus = true;
}

bool Menu::IsFocused() const
{
	return isFocused;
}

bool Menu::IsHovered() const
{
	return isHovered;
}

Vector2Int Menu::GetWindowSize() const
{
	return windowSize;
}

Vector2Int Menu::GetWindowPosition() const
{
	return windowPosition;
}

Vector2Int Menu::GetMousePosition() const
{
	return mousePosition;
}

void Menu::SetActive(bool active)
{
	isActive = active;
	if (isActive) 
	{
		OnOpen();
	}
}

bool Menu::GetActive() const
{
	return isActive;
}

void Menu::OnStartDrawing()
{
	const ImVec2 size = ImGui::GetContentRegionAvail();
	startAvailableSize = Vector2Int(size.x, size.y);
	windowSize = startAvailableSize;
	if (forceFocus)
	{
		ImGui::SetWindowFocus();
		isFocused = true;
		forceFocus = false;
	}
}

void Menu::ResetWindowValues()
{
	windowPosition = Vector2Int(0, 0);
	mousePosition = Vector2Int(0, 0);
	windowSize = Vector2Int(0, 0);
	startAvailableSize = Vector2Int(0, 0);
	isHovered = false;
	isFocused = false;
}

void Menu::CalculateWindowValues()
{
	const ImVec2 imguiWindowPos = ImGui::GetWindowPos();
	const ImVec2 imguiMousePos = ImGui::GetMousePos();
	windowPosition = Vector2Int((int)imguiWindowPos.x, (int)imguiWindowPos.y);
	oldMousePosition = mousePosition;
	mousePosition = Vector2Int((int)imguiMousePos.x, (int)(imguiMousePos.y - (ImGui::GetWindowSize().y - startAvailableSize.y))) - windowPosition;
	isFocused = ImGui::IsWindowFocused();
	isHovered = ImGui::IsWindowHovered();
}
