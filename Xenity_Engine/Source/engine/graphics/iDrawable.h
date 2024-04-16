#pragma once
#include <engine/api.h>

#include <memory>
#include <vector>
#include <engine/component.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include "iDrawableTypes.h"
#include "render_command.h"

class Material;

class API IDrawable : public Component
{
public:
	IDrawable();

	virtual ~IDrawable();

	IDrawableTypes type = IDrawableTypes::Draw_3D;
	virtual int GetDrawPriority() const = 0;
	void RemoveReferences() override;

	/**
	* Create the render commands
	*/
	virtual void CreateRenderCommands(RenderBatch& renderBatch) { }

	/**
	* Draw the command
	*/
	virtual void DrawCommand(const RenderCommand & renderCommand) {};
};
