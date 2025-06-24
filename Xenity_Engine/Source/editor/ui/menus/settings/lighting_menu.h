// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>

#include <engine/event_system/event_system.h>

class SkyBox;

class LightingMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	void OnValueChanged();

	Event<> m_onValueChangedEvent;
};

