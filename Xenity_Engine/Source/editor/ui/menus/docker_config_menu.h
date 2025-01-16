// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"

#include <editor/compiler.h>

#include <engine/event_system/event_system.h>

class DockerConfigMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Refresh();
private:
	void SetDockerState(const DockerState state);
	DockerState currentDockerState = DockerState::NOT_INSTALLED;
	Event<DockerState> dockerStateEvent;
};

