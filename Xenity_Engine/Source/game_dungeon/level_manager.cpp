#include "level_manager.h"
#include <xenity.h>
#include "player.h"

Level::Level()
{
}

ReflectiveData Level::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, levelGo, "levelGo", true);
	Reflective::AddVariable(reflectedVariables, spawnPoint, "spawnPoint", true);
	Reflective::AddVariable(reflectedVariables, coins, "coins", true);
	return reflectedVariables;
}

LevelManager::LevelManager()
{
	componentName = "LevelManager";
}

void LevelManager::Start()
{
}

void LevelManager::Update()
{
}

Level* LevelManager::GetCurrentLevel()
{
	Level* level = nullptr;

	if (currentLevel == 0)
		level = &level1;
	else if (currentLevel == 1)
		level = &level2;

	return level;
}

void LevelManager::ResetPlayerPosition()
{
	Level* level = GetCurrentLevel();
	if (player.lock() && level)
		player.lock()->GetGameObject()->GetTransform()->SetPosition(level->spawnPoint.lock()->GetPosition());
}

void LevelManager::RestartLevel()
{
	ResetPlayerPosition();
	player.lock()->coinCount = 0;
	Level* level = GetCurrentLevel();
	for (int i = 0; i < level->coins.size(); i++)
	{
		level->coins[i].lock()->SetActive(true);
	}
}

void LevelManager::LoadNextLevel()
{
	Level* level = nullptr;
	level = GetCurrentLevel();
	if (level && level->levelGo.lock())
		level->levelGo.lock()->SetActive(false);

	currentLevel++;
	level = GetCurrentLevel();
	if (level && level->levelGo.lock())
		level->levelGo.lock()->SetActive(true);
}

ReflectiveData LevelManager::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, player, "player", true);
	Reflective::AddVariable(reflectedVariables, level1, "level1", true);
	Reflective::AddVariable(reflectedVariables, level2, "level2", true);

	//Reflective::AddVariable(reflectedVariables, levels, "levels", true);
	return reflectedVariables;
}
