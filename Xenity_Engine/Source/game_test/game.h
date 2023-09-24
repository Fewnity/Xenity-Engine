#if defined(EXPORT)
//#define API __declspec(dllexport)
#define API2
#elif defined(IMPORT)
#define API2 __declspec(dllexport)
#else
#define API2
#endif

#pragma once

#include <memory>
#include "../engine/game_interface.h"

class GameObject;
class TextRendererCanvas;
class Tilemap;
class AudioSource;

class Game : public GameInterface
{
public:
	static Game *game;
	static Game *GetGame()
	{
		return game;
	}

	void Start() override;
	void Update() override;

private:
	void LoadGameData();
	std::weak_ptr<GameObject> spriteGo4;
	std::weak_ptr<TextRendererCanvas> debugTextRenderer;
	std::weak_ptr<GameObject> cameraGO;
	std::weak_ptr<GameObject> cameraPivot;
	std::weak_ptr<GameObject> tilemapGO;
	std::weak_ptr<Tilemap> tilemap;
	std::weak_ptr<AudioSource> myAudioSource;
};

extern "C"
{
	API2 GameInterface* CreateGame();
};