#pragma once

#include <string>

class Texture;

class UnitData {
public:
	UnitData() = delete;
	UnitData(int index, Texture* selectionTexture, Texture* bulletTexture);

	Texture* textures[4];
	Texture* selectionTexture = nullptr;
	Texture* bulletTexture = nullptr;
	float health = 0;
	float speed = 0;
	float price = 0;
	float damage = 0;
	float fireRate = 0;
	float movementSpeed = 0;
	bool rotateWhenMoving = false;
	std::string name = "";
	bool used = false;
};