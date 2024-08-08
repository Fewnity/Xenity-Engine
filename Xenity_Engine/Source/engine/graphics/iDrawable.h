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
	IDrawable();

	virtual ~IDrawable();

protected:
	friend class Graphics;

	void RemoveReferences() override;

	/**
	* @brief Create the render commands
	*/
	virtual void CreateRenderCommands(RenderBatch& renderBatch) { }

	/**
	* @brief Draw the command
	*/
	virtual void DrawCommand(const RenderCommand & renderCommand) {};

	virtual void OnNewRender() {};
};
