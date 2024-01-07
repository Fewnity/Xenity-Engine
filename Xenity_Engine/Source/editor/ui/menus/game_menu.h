#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class GameMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool needUpdateCamera = false;
private:
	bool isLastFrameOpened = false;
	void DrawNoCameraText();
	Vector2Int lastSize = Vector2Int(0);
};

