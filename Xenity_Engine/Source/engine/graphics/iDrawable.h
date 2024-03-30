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

	//virtual void Draw() {};
	IDrawableTypes type = IDrawableTypes::Draw_3D;
	virtual int GetDrawPriority() const = 0;
	bool isTransparent = false;
	void RemoveReferences() override;

	virtual void CreateRenderCommands(RenderBatch& renderBatch) { }
	virtual void DrawCommand(const RenderCommand & renderCommand) {};
};
