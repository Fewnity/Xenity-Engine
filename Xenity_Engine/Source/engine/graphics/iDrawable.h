// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/component.h>
#include "render_command.h"

class Material;

class API IDrawable : public Component
{
public:
	virtual ~IDrawable() = default;

protected:
	friend class Graphics;

	void RemoveReferences() override;

	/**
	* @brief Create the render commands
	*/
	virtual void CreateRenderCommands(RenderBatch& renderBatch) = 0;

	/**
	* @brief Draw the command
	*/
	virtual void DrawCommand(const RenderCommand & renderCommand) = 0;

	virtual void OnNewRender() {};
};
