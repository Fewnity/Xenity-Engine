#pragma once

#include <engine/vectors/vector2_int.h>

enum class MenuGroup
{
	Menu_Select_Project,
	Menu_Create_Project,
	Menu_Editor
};

class Menu : public std::enable_shared_from_this<Menu>
{
public:
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
	virtual bool IsFocused() const;

	/**
	* @brief Return if the menu is hovered
	*/
	virtual bool IsHovered() const;

	/**
	* @brief Return the window size
	*/
	virtual Vector2Int GetWindowSize() const;

	/**
	* @brief Return the window position
	*/
	virtual Vector2Int GetWindowPosition() const;

	/**
	* @brief Return the mouse position
	*/
	virtual Vector2Int GetMousePosition() const;

	/**
	* @brief Activate or deactivate the menu
	* @param active: true to activate, false to deactivate
	*/
	virtual void SetActive(bool active);

	/**
	* @brief Return if the menu is active
	*/
	virtual bool GetActive() const;

	MenuGroup group = MenuGroup::Menu_Editor;
	int id = 0;

protected:

	/**
	* @brief Called when the menu starts drawing
	*/
	virtual void OnStartDrawing();

	/**
	* @brief Reset the window values (size, position, etc.)
	*/
	virtual void ResetWindowValues();

	/**
	* @brief Calculate the window values (size, position, etc.)
	*/
	virtual void CalculateWindowValues();

	bool isHovered = false;
	bool isFocused = false;
	bool forceFocus = false;
	bool isActive = true;

	Vector2Int windowSize = Vector2Int(0);
	Vector2Int windowPosition = Vector2Int(0);
	Vector2Int mousePosition = Vector2Int(0);
	Vector2Int oldMousePosition = Vector2Int(0);
	Vector2Int startAvailableSize = Vector2Int(0);
};

