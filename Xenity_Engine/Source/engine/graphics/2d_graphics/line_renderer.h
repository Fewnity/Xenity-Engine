#pragma once

class Texture;
class Material;

#include "../../component.h"
#include "../iDrawable.h"
#include "../../vectors/vector4.h"
#include "../../vectors/vector3.h"
#include "../color/color.h"

class LineRenderer : public IDrawable
{
	public:
		LineRenderer();
		LineRenderer(float width, Material* material);
		~LineRenderer();

		//void SetSprite(const Texture* texture);
		int GetDrawPriority();

		Material* material = nullptr;
		Color color = Color();
		//Vector4 color = Vector4(1,1,1,1);
		Vector3 startPosition = Vector3(0, 0, 0);
		Vector3 endPosition = Vector3(0, 0, 0);
		float width = 1;
		int orderInLayer = 0;

	private:
		void Draw();
};

