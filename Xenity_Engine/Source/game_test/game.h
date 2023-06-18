#pragma once

#include <memory>

class GameObject;
class TextRendererCanvas;
class Tilemap;

class Game
{
public:
	static Game *game;
	static Game *GetGame()
	{
		return game;
	}

	void Start();
	void Update();
	void LoadGameData();

private:
	std::weak_ptr < GameObject>spriteGo4;
	TextRendererCanvas *debugTextRenderer = nullptr;
	std::weak_ptr < GameObject>cameraGO;

	std::weak_ptr < GameObject>cameraPivot;

	std::weak_ptr < GameObject>tilemapGO;
	Tilemap *tilemap = nullptr;
};
