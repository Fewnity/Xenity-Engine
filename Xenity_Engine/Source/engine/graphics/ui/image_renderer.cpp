#include "image_renderer.h"

#include <engine/graphics/texture/texture.h>

ReflectiveData ImageRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, image, "image", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	return reflectedVariables;
}
