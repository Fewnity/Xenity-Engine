#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class GameMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool needUpdateCamera = false;
	Vector2Int lastSize = Vector2Int(1280,720);
private:
	bool isLastFrameOpened = false;
	void DrawNoCameraText();
};

