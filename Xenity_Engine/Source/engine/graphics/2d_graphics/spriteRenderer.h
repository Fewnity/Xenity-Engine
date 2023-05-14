#pragma once

class Texture;
class Material;

#include "../../component.h"
#include "../iDrawable.h"
#include "../color/color.h"

class SpriteRenderer : public IDrawable
{
	public:
		SpriteRenderer();
		~SpriteRenderer();

		int GetDrawPriority();

		void SetOrderInLayer(int orderInLayer) 
		{
			this->orderInLayer = orderInLayer;
			needReorder = true;
		}

		int GetOrderInLayer() const
		{
			return orderInLayer;
		}

		Material* material = nullptr;
		Texture* texture = nullptr;
		Color color = Color();

	private:
		void Draw();
		int orderInLayer = 0;
};

