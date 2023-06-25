#pragma once

#include <memory>

class GameObject;
class TextRendererCanvas;
class Tilemap;
class AudioSource;

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
	std::weak_ptr<GameObject> spriteGo4;
	std::weak_ptr<TextRendererCanvas> debugTextRenderer;
	std::weak_ptr<GameObject> cameraGO;
	std::weak_ptr<GameObject> cameraPivot;
	std::weak_ptr<GameObject> tilemapGO;
	std::weak_ptr<Tilemap> tilemap;
	std::weak_ptr<AudioSource> myAudioSource;
};
