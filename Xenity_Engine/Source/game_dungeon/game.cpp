#include "game.h"
#include <xenity.h>

#include <engine/class_registry/class_registry.h>
#include "player.h"
#include "camera_follow.h"
#include "coin.h"
#include "level_manager.h"
#include "spike.h"
#include "chest.h"
#include "enemy.h"
#include "animated_light.h"

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	ClassRegistry::AddComponentClass<Player>("Player");
	ClassRegistry::AddComponentClass<CameraFollow>("CameraFollow");
	ClassRegistry::AddComponentClass<Coin>("Coin");
	ClassRegistry::AddComponentClass<Spike>("Spike");
	ClassRegistry::AddComponentClass<LevelManager>("LevelManager");
	ClassRegistry::AddComponentClass<Chest>("Chest");
	ClassRegistry::AddComponentClass<Enemy>("Enemy");
	ClassRegistry::AddComponentClass<AnimatedLight>("AnimatedLight");
}

GameInterface *CreateGame()
{
	return new Game();
}