#pragma once

#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>

class Texture;

class API ImageRenderer : public IDrawable
{
public:
	std::shared_ptr<Texture> image;
	Color color;
private:
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;
};

