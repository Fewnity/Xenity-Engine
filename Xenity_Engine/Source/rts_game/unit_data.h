#pragma once

class Texture;

class UnitData {
public:
	UnitData() = delete;
	UnitData(int index, Texture* selectionTexture);

	Texture* textures[4];
	Texture* selectionTexture = nullptr;
	float health = 0;
	float speed = 0;
	float price = 0;
};