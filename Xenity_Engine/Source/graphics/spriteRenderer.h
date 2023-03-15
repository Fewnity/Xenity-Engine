#pragma once

class Texture;
class Material;

#include "../component.h"
#include "iDrawable.h"

class SpriteRenderer : public Component, public IDrawable
{
	public:
		SpriteRenderer();
		SpriteRenderer(const Texture* texture, Material* material);
		~SpriteRenderer();


		const Texture* texture = nullptr;
		Material* material = nullptr;

	private:
		void Draw();
};

