#pragma once

#include <vector>
#include <memory>

class Scene;
class Component;

class SceneManager
{
public:
	static void LoadScene(Scene *scene);
	static Scene* openedScene;
private:
	static std::vector<std::shared_ptr<Component>> allCreatedComponents;
	static int allCreatedComponentsCount;
};

