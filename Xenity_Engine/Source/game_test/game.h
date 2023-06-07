#pragma once

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
	GameObject *spriteGo4 = nullptr;
	TextRendererCanvas *debugTextRenderer = nullptr;
	GameObject *cameraGO = nullptr;

	GameObject *cameraPivot = nullptr;

	GameObject *tilemapGO = nullptr;
	Tilemap *tilemap = nullptr;
};
