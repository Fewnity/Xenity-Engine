#pragma once

class Texture;
class Material;

#include "../component.h"
#include "iDrawable.h"
#include "../vectors/vector4.h"

class SpriteRenderer : public IDrawable
{
	public:
		SpriteRenderer();
		SpriteRenderer(const Texture* texture, Material* material);
		~SpriteRenderer();

		//void SetSprite(const Texture* texture);

		Material* material = nullptr;
		Vector4 color = Vector4(1,1,1,1);

	private:
		const Texture* texture = nullptr;
		void Draw();
};

