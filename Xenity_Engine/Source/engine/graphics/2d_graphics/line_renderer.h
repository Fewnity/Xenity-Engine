#pragma once
#include <engine/api.h>

class Texture;
class Material;
class MeshData;

#include <engine/graphics/iDrawable.h>

#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/component.h>

#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/graphics/color/color.h>

class API LineRenderer : public IDrawable
{
public:
	LineRenderer();
	~LineRenderer();
	ReflectiveData GetReflectiveData() override;

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

	Vector3 startPosition = Vector3(0, 0, 0);
	Vector3 endPosition = Vector3(0, 0, 0);
	float width = 1;

private:
	void Draw() override;
	Color color = Color();
	int orderInLayer = 0;
	std::shared_ptr <MeshData> meshData = nullptr;
};

