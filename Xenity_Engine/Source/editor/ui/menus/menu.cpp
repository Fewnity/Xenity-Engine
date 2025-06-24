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
	m_forceFocus = true;
}

bool Menu::IsFocused() const
{
	return m_isFocused;
}

bool Menu::IsHovered() const
{
	return m_isHovered;
}

Vector2 Menu::GetWindowSize() const
{
	return m_windowSize;
}

Vector2 Menu::GetWindowPosition() const
{
	return m_windowPosition;
}

Vector2 Menu::GetMousePosition() const
{
	return m_mousePosition;
}

void Menu::SetActive(bool active)
{
	m_isActive = active;
	m_previousIsActive = active;
	if (m_isActive)
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
	return m_isActive;
}

void Menu::OnClose()
{
	Editor::OnMenuActiveStateChange(name, m_isActive, id);
}

void Menu::OnStartDrawing()
{
	const ImVec2 size = ImGui::GetContentRegionAvail();
	m_startAvailableSize = Vector2(size.x, size.y);
	m_windowSize = m_startAvailableSize;
	if (m_forceFocus)
	{
		ImGui::SetWindowFocus();
		m_isFocused = true;
		m_forceFocus = false;
	}
}

void Menu::CheckOnCloseEvent() 
{
	if (m_isActive != m_previousIsActive)
	{
		if (!m_isActive)
		{
			OnClose();
		}
		m_previousIsActive = m_isActive;
	}
}

void Menu::ResetWindowValues()
{
	m_windowPosition = Vector2(0, 0);
	m_mousePosition = Vector2(0, 0);
	m_windowSize = Vector2(0, 0);
	m_startAvailableSize = Vector2(0, 0);
	m_isHovered = false;
	m_isFocused = false;

	CheckOnCloseEvent();
}

void Menu::CalculateWindowValues()
{
	const ImVec2 imguiWindowPos = ImGui::GetWindowPos();
	const ImVec2 imguiMousePos = ImGui::GetMousePos();
	m_windowPosition = Vector2(imguiWindowPos.x, imguiWindowPos.y);
	m_oldMousePosition = m_mousePosition;
	m_mousePosition = Vector2(imguiMousePos.x, (imguiMousePos.y - (ImGui::GetWindowSize().y - m_startAvailableSize.y))) - m_windowPosition;
	m_isFocused = ImGui::IsWindowFocused();
	m_isHovered = ImGui::IsWindowHovered();

	CheckOnCloseEvent();
}
