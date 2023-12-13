#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class GameMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool isHovered = false;
	Vector2Int windowSize = Vector2Int(0);
	Vector2Int windowPosition = Vector2Int(0);
	Vector2Int mousePosition = Vector2Int(0);
private:
	bool isLastFrameOpened = false;
};

