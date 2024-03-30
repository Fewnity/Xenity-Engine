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
	void OnDisabled() override;
	void OnEnabled() override;
	void CreateRenderCommands(RenderBatch& renderBatch) override;
	void DrawCommand(const RenderCommand& renderCommand) override;
	Vector2Int lastSize = 0;
};

