#pragma once

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector2_int.h>

class API Canvas : public IDrawable
{
public:
	Canvas();
	~Canvas();
	void OnDrawGizmos() override;
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* [Internal]
	*/
	int GetDrawPriority() const override;

private:

	/**
	* Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	Vector2Int lastSize = 0;
};

