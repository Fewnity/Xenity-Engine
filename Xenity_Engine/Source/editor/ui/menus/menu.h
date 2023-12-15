#pragma once

#include <engine/vectors/vector2_int.h>

enum MenuGroup
{
	Menu_Select_Project,
	Menu_Create_Project,
	Menu_Editor
};

class Menu
{
public:
	virtual void Init() = 0;
	virtual void Draw() = 0;
	virtual void Focus();
	virtual bool IsFocused();
	virtual bool IsHovered();
	virtual Vector2Int GetWindowSize();
	virtual Vector2Int GetWindowPosition();
	virtual Vector2Int GetMousePosition();
	virtual void SetActive(bool active);
	virtual bool GetActive();
	MenuGroup group = Menu_Editor;
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

