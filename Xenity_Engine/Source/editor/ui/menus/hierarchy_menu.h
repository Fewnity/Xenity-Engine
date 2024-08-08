// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include "menu.h"

class GameObject;

class HierarchyMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	std::weak_ptr<GameObject> rightClickedElement;
	bool firstClickedInWindow = false;
};

