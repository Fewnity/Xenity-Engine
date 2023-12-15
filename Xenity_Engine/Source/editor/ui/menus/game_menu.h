#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class GameMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	bool isLastFrameOpened = false;
	void DrawNoCameraText();
};

