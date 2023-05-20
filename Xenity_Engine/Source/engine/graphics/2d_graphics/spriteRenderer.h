#pragma once

class Texture;
class Material;

#include "../../component.h"
#include "../iDrawable.h"
#include "../color/color.h"
#include "../../engine.h"

class SpriteRenderer : public IDrawable
{
	public:
		SpriteRenderer();
		~SpriteRenderer();

		int GetDrawPriority() const;

		void SetOrderInLayer(int orderInLayer) 
		{
			this->orderInLayer = orderInLayer;
			Engine::drawOrderListDirty = true;
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

