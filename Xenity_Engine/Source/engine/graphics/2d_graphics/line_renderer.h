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

class LineRenderer : public IDrawable
{
	public:
		LineRenderer();
		~LineRenderer();

		int GetDrawPriority() const;

		//Material* material = nullptr;
		Color color = Color();

		Vector3 startPosition = Vector3(0, 0, 0);
		Vector3 endPosition = Vector3(0, 0, 0);
		float width = 1;

		void SetOrderInLayer(int orderInLayer)
		{
			this->orderInLayer = orderInLayer;
			Engine::drawOrderListDirty = true;
		}

		int GetOrderInLayer() const
		{
			return orderInLayer;
		}

	private:
		void Draw();
		int orderInLayer = 0;
		MeshData* meshData = nullptr;
};

