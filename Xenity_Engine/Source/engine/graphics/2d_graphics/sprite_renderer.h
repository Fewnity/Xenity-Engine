#pragma once
#include <engine/api.h>

class Texture;

#include <engine/component.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>
#include <memory>

class API SpriteRenderer : public IDrawable
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer);

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}


	void SetColor(const Color& color)
	{
		this->color = color;
	}
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	void SetMaterial(std::shared_ptr <Material> material);
	void SetTexture(std::shared_ptr <Texture> texture);

	void OnDisabled() override;
	void OnEnabled() override;

private:
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;
	Color color = Color();

	void DrawCommand(const RenderCommand& renderCommand) override;
	int orderInLayer = 0;
};
