#pragma once

#include <memory>
#include "../engine/engine.h"
#include "../engine/game_elements/gameobject.h"
#include <json.hpp>

class Component;
class Reflection;
class FileReference;

class Editor
{
public:
	static void Start();
	static void Update();
	static void Draw();
	static void CreateEmpty();
	static void CreateEmptyChild();
	static void SaveScene();
	static void JsonToReflection(nlohmann::json j, Reflection& component);
	static nlohmann::json ReflectiveToJson(Reflection& relection);
	static std::vector<FileReference*> fileRefs;

	template <typename T>
	static std::shared_ptr<T> AddComponentToSelection()
	{
		if (auto selectedGO = Engine::selectedGameObject.lock())
		{
			return selectedGO->AddComponent<T>();
		}
		else
		{
			return nullptr;
		}
	}
private:
	static std::weak_ptr<GameObject> cameraGO;
};

