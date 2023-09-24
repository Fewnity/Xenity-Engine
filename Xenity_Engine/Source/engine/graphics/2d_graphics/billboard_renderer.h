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

#include "../../component.h"
#include "../iDrawable.h"
#include "../color/color.h"
#include "../../engine.h"
#include <memory>

class API BillboardRenderer : public IDrawable
{
public:
	BillboardRenderer();
	~BillboardRenderer();
	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}

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
