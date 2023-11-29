#pragma once

class Player;

#include <engine/monobehaviour.h>
#include <engine/component.h>

class Level : public Reflective
{
public:
	Level();
	std::weak_ptr<GameObject> levelGo;
	std::weak_ptr<Transform> spawnPoint;
	std::vector<std::weak_ptr<GameObject>> coins;
	ReflectiveData GetReflectiveData() override;
};

class LevelManager : public MonoBehaviour
{
public:
	LevelManager();
	void Start() override;
	void Update() override;
	void RestartLevel();
	void LoadNextLevel();
	Level *GetCurrentLevel();
	void ResetPlayerPosition();

	std::weak_ptr<Player> player;

	ReflectiveData GetReflectiveData() override;

	int currentLevel = 0;
	Level level1;
	Level level2;
	std::vector<Level> levels;
	std::vector<int> ints;
	std::shared_ptr<Scene> myTestScene;
private:
};
