#pragma once

#include <memory>
#include "../engine/engine.h"
#include "../engine/game_elements/gameobject.h"
#include <json.hpp>
#include <unordered_map>

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
	static nlohmann::json MapToJson(std::unordered_map<std::string, Variable> theMap, nlohmann::json json);
	static void JsonToMap(std::unordered_map<std::string, Variable>, nlohmann::json json);

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

