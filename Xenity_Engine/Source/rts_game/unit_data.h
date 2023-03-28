#pragma once

class Texture;

class UnitData {
public:
	UnitData() = delete;
	UnitData(int index);

	Texture* textures[4];
	float health = 0;
	float speed = 0;
	float price = 0;
};