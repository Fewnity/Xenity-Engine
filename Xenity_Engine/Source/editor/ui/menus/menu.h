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
	virtual void Init() = 0;
	virtual void Draw() = 0;
	virtual void Focus();
	virtual bool IsFocused() const;
	virtual bool IsHovered() const;
	virtual Vector2Int GetWindowSize() const;
	virtual Vector2Int GetWindowPosition() const;
	virtual Vector2Int GetMousePosition() const;
	virtual void SetActive(bool active);
	virtual bool GetActive() const;
	MenuGroup group = MenuGroup::Menu_Editor;
	int id = 0;

protected:
	virtual void OnStartDrawing();
	virtual void ResetWindowValues();
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

