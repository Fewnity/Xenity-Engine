#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class SceneMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool isHovered = false;
	bool isFocused = false;
	Vector2Int windowSize = Vector2Int(0);
	Vector2Int windowPosition = Vector2Int(0);
	Vector2Int mousePosition = Vector2Int(0);
private:
	void DrawToolWindow();
	void SetButtonColor(bool isSelected);
};

