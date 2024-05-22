// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include "menu.h"

class SkyBox;

class LightingMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
};

