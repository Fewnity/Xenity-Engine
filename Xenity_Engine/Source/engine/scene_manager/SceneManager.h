#pragma once

#include <vector>

class Scene;

class SceneManager {
public:
	static void LoadScene(Scene* scene);
	static void LoadSceneAdditive(Scene* scene);
	static void UnloadScene(Scene* scene);
	static void UnloadScenes();

private:
	static std::vector<Scene*> loadedScenes;


};