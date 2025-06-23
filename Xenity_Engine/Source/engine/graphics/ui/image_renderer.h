#pragma once

#include <engine/component.h>
#include <engine/graphics/color/color.h>

class Texture;

class API ImageRenderer : public Component
{
public:
	std::shared_ptr<Texture> image;
	Color color;
private:
	ReflectiveData GetReflectiveData() override;
};

