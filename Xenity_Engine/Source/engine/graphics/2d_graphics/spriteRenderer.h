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
		int orderInLayer = 0;

		Material* material = nullptr;
		Texture* texture = nullptr;
		Color color = Color();

	private:
		void Draw();
};

