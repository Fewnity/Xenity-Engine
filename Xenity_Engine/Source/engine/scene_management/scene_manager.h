#pragma once

#include <vector>
#include <memory>
#include <json.hpp>

class Scene;
class Component;

class SceneManager
{
public:
	static void LoadScene(Scene *scene);
	static void SaveScene(bool saveForPlayState);
	static void RestoreScene();
	static void EmptyScene();
	static Scene* openedScene;
private:
	static void LoadScene(nlohmann::json jsonData);
	static std::vector<std::shared_ptr<Component>> allCreatedComponents;
	static int allCreatedComponentsCount;
};

