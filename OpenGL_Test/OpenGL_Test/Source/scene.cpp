#include "scene.h"

void Scene::addGameObject(GameObject* gameObject)
{
	gameObjects.emplace_back(gameObject);
}
