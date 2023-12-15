#pragma once

#include <engine/vectors/vector2_int.h>

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

protected:
	virtual void OnStartDrawing();
	virtual void ResetWindowValues();
	virtual void CalculateWindowValues();

	bool isHovered = false;
	bool isFocused = false;
	bool forceFocus = false;
	Vector2Int windowSize = Vector2Int(0);
	Vector2Int windowPosition = Vector2Int(0);
	Vector2Int mousePosition = Vector2Int(0);
	Vector2Int oldMousePosition = Vector2Int(0);
	Vector2Int startAvailableSize = Vector2Int(0);
};

