// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <editor/ui/menus/menu.h>

#include <imgui/imgui.h>

#include <editor/editor.h>

#include <engine/math/vector2.h>

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

Vector2 Menu::GetWindowSize() const
{
	return windowSize;
}

Vector2 Menu::GetWindowPosition() const
{
	return windowPosition;
}

Vector2 Menu::GetMousePosition() const
{
	return mousePosition;
}

void Menu::SetActive(bool active)
{
	isActive = active;
	previousIsActive = active;
	if (isActive) 
	{
		OnOpen();
	}
	else
	{
		OnClose();
	}
}

bool Menu::IsActive() const
{
	return isActive;
}

void Menu::OnClose()
{
	Editor::OnMenuActiveStateChange(name, isActive, id);
}

void Menu::OnStartDrawing()
{
	const ImVec2 size = ImGui::GetContentRegionAvail();
	startAvailableSize = Vector2(size.x, size.y);
	windowSize = startAvailableSize;
	if (forceFocus)
	{
		ImGui::SetWindowFocus();
		isFocused = true;
		forceFocus = false;
	}
}

void Menu::CheckOnCloseEvent() 
{
	if (isActive != previousIsActive)
	{
		if (!isActive)
		{
			OnClose();
		}
		previousIsActive = isActive;
	}
}

void Menu::ResetWindowValues()
{
	windowPosition = Vector2(0, 0);
	mousePosition = Vector2(0, 0);
	windowSize = Vector2(0, 0);
	startAvailableSize = Vector2(0, 0);
	isHovered = false;
	isFocused = false;

	CheckOnCloseEvent();
}

void Menu::CalculateWindowValues()
{
	const ImVec2 imguiWindowPos = ImGui::GetWindowPos();
	const ImVec2 imguiMousePos = ImGui::GetMousePos();
	windowPosition = Vector2(imguiWindowPos.x, imguiWindowPos.y);
	oldMousePosition = mousePosition;
	mousePosition = Vector2(imguiMousePos.x, (imguiMousePos.y - (ImGui::GetWindowSize().y - startAvailableSize.y))) - windowPosition;
	isFocused = ImGui::IsWindowFocused();
	isHovered = ImGui::IsWindowHovered();

	CheckOnCloseEvent();
}
