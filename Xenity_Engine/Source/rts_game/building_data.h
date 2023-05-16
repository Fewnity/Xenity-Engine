#pragma once

#include "building_type_enum.h"

class Texture;

class BuildingData {
public:
	BuildingData() = delete;
	BuildingData(int index, Texture* selectionTexture);

	Texture* textures[4];
	Texture* selectionTexture = nullptr;
	float health = 0;
	float price = 0;
	BuildingType type;
};