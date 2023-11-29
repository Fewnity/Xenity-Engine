#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

class Texture;
// class Material;

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

	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer);

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;

	void SetColor(const Color& color)
	{
		this->color = color;
	}

private:
	Color color = Color();
	void Draw() override;
	int orderInLayer = 0;
};
