#pragma once
#include <engine/api.h>

class Texture;

#include <engine/graphics/iDrawable.h>

#include <engine/engine.h>
#include <engine/component.h>

#include <engine/graphics/color/color.h>

#include <memory>

class API BillboardRenderer : public IDrawable
{
public:
	BillboardRenderer();
	~BillboardRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer);

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* @brief Set the color of the sprite
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* @brief Get the material of the sprite
	*/
	std::shared_ptr <Material> GetMaterial()
	{
		return material;
	}

	/**
	* @brief Get the texture of the sprite
	*/
	std::shared_ptr <Texture> GetTexture()
	{
		return texture;
	}

	/**
	* @brief Set the material of the sprite
	*/
	void SetMaterial(std::shared_ptr <Material> material);
	
	/**
	* @brief Set the texture of the sprite
	*/
	void SetTexture(std::shared_ptr <Texture> texture);

private:

	/**
	* @brief Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* @brief Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	Color color = Color();
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;
	int orderInLayer = 0;
};
