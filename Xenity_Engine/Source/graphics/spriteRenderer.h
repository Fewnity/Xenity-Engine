#pragma once

class Texture;
class Shader;

#include "../component.h"
#include "iDrawable.h"

class SpriteRenderer : public Component, public IDrawable
{
	public:
		SpriteRenderer();
		~SpriteRenderer();

		void Draw();

		Texture* texture = nullptr;
		Shader* shader = nullptr;

	private:
};

