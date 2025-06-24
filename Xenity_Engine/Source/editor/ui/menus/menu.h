// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/math/vector2.h>

enum class MenuGroup
{
	Menu_Select_Project,
	Menu_Create_Project,
	Menu_Editor
};

class Menu : public std::enable_shared_from_this<Menu>
{
public:
	virtual ~Menu() = default;

	/**
	* @brief Initializes the menu, called once when the menu is created
	*/
	virtual void Init() = 0;

	/**
	* @brief Updates the menu, called every frame
	*/
	virtual void Draw() = 0;

	/**
	* @brief Focuses the menu
	*/
	virtual void Focus();

	/**
	* @brief Return if the menu is focused
	*/
	[[nodiscard]] bool IsFocused() const;

	/**
	* @brief Return if the menu is hovered
	*/
	[[nodiscard]] bool IsHovered() const;

	/**
	* @brief Return the window size
	*/
	[[nodiscard]] Vector2 GetWindowSize() const;

	/**
	* @brief Return the window position
	*/
	[[nodiscard]] Vector2 GetWindowPosition() const;

	/**
	* @brief Return the mouse position
	*/
	[[nodiscard]] Vector2 GetMousePosition() const;

	/**
	* @brief Activate or deactivate the menu
	* @param active: true to activate, false to deactivate
	*/
	virtual void SetActive(bool active);

	/**
	* @brief Return if the menu is active
	*/
	[[nodiscard]] virtual bool IsActive() const;

	MenuGroup group = MenuGroup::Menu_Editor;
	int id = 0;
	std::string name;

	virtual void OnOpen() {}

	virtual void OnClose();

protected:

	/**
	* @brief Called when the menu starts drawing
	*/
	virtual void OnStartDrawing();

	/**
	* @brief Reset the window values (size, position, etc.)
	*/
	virtual void ResetWindowValues();

	void CheckOnCloseEvent();

	/**
	* @brief Calculate the window values (size, position, etc.)
	*/
	virtual void CalculateWindowValues();

	bool m_isHovered = false;
	bool m_isFocused = false;
	bool m_forceFocus = false;
	bool m_isActive = true;
	bool m_previousIsActive = true;

	Vector2 m_windowSize = Vector2(0);
	Vector2 m_windowPosition = Vector2(0);
	Vector2 m_mousePosition = Vector2(0);
	Vector2 m_oldMousePosition = Vector2(0);
	Vector2 m_startAvailableSize = Vector2(0);
};

