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

	std::shared_ptr <Material> GetMaterial()
	{
		return material;
	}

	std::shared_ptr <Texture> GetTexture()
	{
		return texture;
	}

	void SetMaterial(std::shared_ptr <Material> material);
	void SetTexture(std::shared_ptr <Texture> texture);

private:
	void OnDisabled() override;
	void OnEnabled() override;
	void CreateRenderCommands(RenderBatch& renderBatch) override;
	void DrawCommand(const RenderCommand& renderCommand) override;

	Color color = Color();
	int orderInLayer = 0;
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;
};
