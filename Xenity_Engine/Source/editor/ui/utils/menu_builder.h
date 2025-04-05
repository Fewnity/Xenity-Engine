// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <string>
#include <functional>

enum class RightClickMenuState
{
	Closed,
	JustOpened,
	Opened,
};

class RightClickMenuItem
{
public:
	~RightClickMenuItem();

	/**
	* @brief Add an item to the menu
	* @param title The title of the item
	* @param onClickFunction The function to be called when the item is clicked
	* @return The item
	*/
	RightClickMenuItem* AddItem(const std::string& title, std::function<void()> onClickFunction);

	/**
	* @brief Add an item to the menu
	* @param title The title of the item
	* @return The item
	*/
	RightClickMenuItem* AddItem(const std::string& title);

	/**
	* @brief Set the title of the item
	* @param title The title of the item
	*/
	void SetTitle(const std::string& title) 
	{
		this->text = title;
	}

	/**
	* @brief Set the enabled state of the item
	* @param isEnabled The enabled state of the item
	*/
	void SetIsEnabled(const bool isEnabled) 
	{
		this->isEnabled = isEnabled;
	}

	/**
	* @brief Set the visible state of the item
	* @param isVisible The visible state of the item
	*/
	void SetIsVisible(const bool isVisible) 
	{
		this->isVisible = isVisible;
	}

	/**
	* @brief Get the title of the item
	*/
	[[nodiscard]] const std::string& GetTitle() const
	{
		return text;
	}

	/**
	* @brief Get the enabled state of the item
	*/
	[[nodiscard]] bool IsEnabled() const
	{
		return isEnabled;
	}

	/**
	* @brief Get the visible state of the item
	*/
	[[nodiscard]] bool GetIsVisible() const
	{
		return isVisible;
	}

	std::vector<RightClickMenuItem*> onHoverItems;
	std::function<void()> onClicked;

private:
	std::string text;
	bool isEnabled = true;
	bool isVisible = true;
};

class RightClickMenu
{
public:
	RightClickMenu() = delete;
	RightClickMenu(const std::string& uniqueName);
	~RightClickMenu();

	/**
	* @brief Check if the menu should be opened or closed
	* @param blockOpen If true, won't be able to be opened
	* @param disableHoveredCheck If true, will disable the item hovered check
	* @return The state of the menu
	*/
	RightClickMenuState Check(const bool blockOpen, bool disableHoveredCheck);

	/**
	* @brief Draw the menu
	*/
	bool Draw();

	/**
	* @brief Add an item to the menu
	* @param title The title of the item
	* @param onClickFunction The function to be called when the item is clicked
	* @return The item
	*/
	RightClickMenuItem* AddItem(const std::string& title, const std::function<void()>& onClickFunction);

	/**
	* @brief Add an item to the menu
	* @param title The title of the item
	* @return The item
	*/
	RightClickMenuItem* AddItem(const std::string& title);

	std::vector<RightClickMenuItem*> items;
private:

	/**
	* @brief Draw the menu recursively
	* @param item The item to draw
	*/
	void DrawRecursive(const RightClickMenuItem& item) const;

	static bool isDrawn;
	static std::string isDrawnName;
	static bool isFocusCorrect;
	std::string nameId;
};