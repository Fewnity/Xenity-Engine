#include "SceneManager.h"


void SceneManager::addGameObjectToScene(GameObject* gameObject) {
	usedScene->addGameObject(gameObject);
}

void SceneManager::setCurrentScene(Scene* scene) {
	usedScene = scene;
}