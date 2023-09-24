#include "game.h"
#include "../xenity.h"

#if defined(__PSP__)
#include <pspkernel.h>
#include <psppower.h>
#include <pspsdk.h>
#include <vram.h>
#include <pspge.h>
#elif defined(__vita__)
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#endif
#include "../engine/class_registry/class_registry.h"
#include "rotate.h"
#include "free_move.h"

Game* Game::game;

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	game = this;
	LoadGameData();

	ClassRegistry::AddComponentClass("Rotate", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<Rotate>(); });
	ClassRegistry::AddComponentClass("FreeMove", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<FreeMove>(); });

	Debug::Print("Game::Start");
}

void Game::LoadGameData()
{
}


/// <summary>
/// Game loop
/// </summary>
void Game::Update()
{
}

GameInterface* CreateGame()
{
	return new Game();
}