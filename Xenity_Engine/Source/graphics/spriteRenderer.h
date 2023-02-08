#pragma once

class Texture;
class Shader;

#include "../component.h"
#include "iDrawable.h"

class SpriteRenderer : public Component, public IDrawable
{
	public:
		SpriteRenderer();
		void Draw();
		float width = 100;
		float height = 100;
		Texture* texture = nullptr;
		Shader* shader = nullptr;
	private:
};

