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

	/**
	* Set the color of the sprite
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* Set the material of the sprite
	*/
	void SetMaterial(std::shared_ptr <Material> material);

	/**
	* Set the texture of the sprite
	*/
	void SetTexture(std::shared_ptr <Texture> texture);

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

	Color color = Color();
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;
	int orderInLayer = 0;
};
