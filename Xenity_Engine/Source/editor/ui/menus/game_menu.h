// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>

class GameMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool needUpdateCamera = false;
	Vector2 lastSize = Vector2(1280,720);
private:
	bool isLastFrameOpened = false;

	/**
	* Draw a message to say there is no camera
	*/
	void DrawNoCameraText();
};

