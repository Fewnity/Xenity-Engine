#include "menu.h"
#include <imgui/imgui.h>

void Menu::Focus()
{
	forceFocus = true;
}

bool Menu::IsFocused()
{
	return isFocused;
}

bool Menu::IsHovered()
{
	return isHovered;
}

Vector2Int Menu::GetWindowSize()
{
	return windowSize;
}

Vector2Int Menu::GetWindowPosition()
{
	return windowPosition;
}

Vector2Int Menu::GetMousePosition()
{
	return mousePosition;
}

void Menu::OnStartDrawing()
{
	ImVec2 size = ImGui::GetContentRegionAvail();
	startAvailableSize = Vector2Int(size.x, size.y);
	windowSize = startAvailableSize;
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
	windowPosition = Vector2Int((int)ImGui::GetWindowPos().x, (int)ImGui::GetWindowPos().y);
	oldMousePosition = mousePosition;
	mousePosition = Vector2Int((int)ImGui::GetMousePos().x, (int)(ImGui::GetMousePos().y - (ImGui::GetWindowSize().y - startAvailableSize.y))) - windowPosition;
	isFocused = ImGui::IsWindowFocused();
	isHovered = ImGui::IsWindowHovered();
}
