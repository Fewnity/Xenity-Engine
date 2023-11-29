#if defined(EXPORT)
// #define API __declspec(dllexport)
#define API2
#elif defined(IMPORT)
#define API2 __declspec(dllexport)
#else
#define API2
#endif

#pragma once

#include <memory>
#include <engine/game_interface.h>

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
};

extern "C"
{
	API2 GameInterface *CreateGame();
};