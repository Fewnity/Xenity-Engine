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
		SpriteRenderer(const Texture* texture, Material* material);
		~SpriteRenderer();

		//void SetSprite(const Texture* texture);
		int GetDrawPriority();
		int orderInLayer = 0;

		Material* material = nullptr;
		Color color = Color();

	private:
		const Texture* texture = nullptr;
		void Draw();
};

