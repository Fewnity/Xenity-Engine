#include "scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::addGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
}
