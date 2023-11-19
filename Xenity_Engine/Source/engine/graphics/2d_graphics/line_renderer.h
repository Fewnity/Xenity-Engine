#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

class Texture;
class Material;
class MeshData;

#include "../../component.h"
#include "../iDrawable.h"
#include "../../vectors/vector4.h"
#include "../../vectors/vector3.h"
#include "../color/color.h"
#include "../../engine.h"
#include "../../game_elements/gameplay_manager.h"

class API LineRenderer : public IDrawable
{
public:
	LineRenderer();
	~LineRenderer();
	ReflectiveData GetReflectiveData() override;

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

	Vector3 startPosition = Vector3(0, 0, 0);
	Vector3 endPosition = Vector3(0, 0, 0);
	float width = 1;

private:
	void Draw() override;
	Color color = Color();
	int orderInLayer = 0;
	std::shared_ptr <MeshData> meshData = nullptr;
};

