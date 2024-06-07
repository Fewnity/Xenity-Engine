#pragma once

#include <memory>
#include <json.hpp>
#include <editor/command/command.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/class_registry/class_registry.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/scene_management/scene_manager.h>

class InspectorDeleteGameObjectCommand : public Command
{
public:
	InspectorDeleteGameObjectCommand() = delete;
	InspectorDeleteGameObjectCommand(std::weak_ptr<GameObject> gameObjectToDestroy);
	void Execute() override;
	void Undo() override;
private:
	struct GameObjectComponent
	{
		nlohmann::json componentData;
		std::string componentName = "";
		bool isEnabled = true;
		uint64_t componentId = 0;
	};

	struct GameObjectChild
	{
		nlohmann::json gameObjectData;
		nlohmann::json transformData;
		uint64_t gameObjectId = 0;
		std::vector<GameObjectChild> children;
		std::vector<GameObjectComponent> components;
	};
	GameObjectChild AddChild(std::shared_ptr<GameObject> child);
	void ReCreateChild(const GameObjectChild& child, std::shared_ptr<GameObject> parent);
	void UpdateChildComponents(const GameObjectChild& child);
	GameObjectChild gameObjectChild;
};

inline InspectorDeleteGameObjectCommand::GameObjectChild InspectorDeleteGameObjectCommand::AddChild(std::shared_ptr<GameObject> child)
{
	GameObjectChild gameObjectChild;
	gameObjectChild.gameObjectId = child->GetUniqueId();
	gameObjectChild.gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(child->GetReflectiveData());
	gameObjectChild.transformData["Values"] = ReflectionUtils::ReflectiveDataToJson(child->GetTransform()->GetReflectiveData());
	for (std::weak_ptr<GameObject> childChild : child->GetChildren())
	{
		gameObjectChild.children.push_back(AddChild(childChild.lock()));
	}

	for (std::shared_ptr<Component> component : child->components)
	{
		GameObjectComponent gameObjectComponent;
		gameObjectComponent.componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(component->GetReflectiveData());
		gameObjectComponent.componentName = component->GetComponentName();
		gameObjectComponent.componentId = component->GetUniqueId();
		gameObjectComponent.isEnabled = component->GetIsEnabled();
		gameObjectChild.components.push_back(gameObjectComponent);
	}
	return gameObjectChild;
}

inline void InspectorDeleteGameObjectCommand::UpdateChildComponents(const GameObjectChild& child)
{
	for (const GameObjectChild& childChild : child.children)
	{
		UpdateChildComponents(childChild);
	}
	for (const GameObjectComponent& childChild : child.components)
	{
		std::shared_ptr<Component> component = FindComponentById(childChild.componentId);
		ReflectionUtils::JsonToReflectiveData(childChild.componentData, component->GetReflectiveData());
		component->OnReflectionUpdated();
	}
}

inline void InspectorDeleteGameObjectCommand::ReCreateChild(const GameObjectChild& child, std::shared_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> newGameObject = CreateGameObject();
	std::shared_ptr<Transform> transformToUpdate = newGameObject->GetTransform();
	ReflectionUtils::JsonToReflectiveData(child.gameObjectData, newGameObject->GetReflectiveData());
	newGameObject->OnReflectionUpdated();
	newGameObject->SetUniqueId(child.gameObjectId);
	if (parent)
		newGameObject->SetParent(parent);
	ReflectionUtils::JsonToReflectiveData(child.transformData, transformToUpdate->GetReflectiveData());
	transformToUpdate->isTransformationMatrixDirty = true;
	transformToUpdate->UpdateWorldValues();
	transformToUpdate->OnReflectionUpdated();
	for (const GameObjectChild& childChild : child.children)
	{
		ReCreateChild(childChild, newGameObject);
	}
	for (const GameObjectComponent& childChild : child.components)
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(childChild.componentName, newGameObject);
		//ReflectionUtils::JsonToReflectiveData(childChild.componentData, component->GetReflectiveData());
		component->SetIsEnabled(childChild.isEnabled);
		component->SetUniqueId(childChild.componentId);

		//child.componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.lock()->GetReflectiveData());
		//this->componentName = componentToUse.lock()->GetComponentName();
	}
}

inline InspectorDeleteGameObjectCommand::InspectorDeleteGameObjectCommand(std::weak_ptr<GameObject> gameObjectToDestroy)
{
	std::shared_ptr<GameObject> gameObjectToDestroyLock = gameObjectToDestroy.lock();
	gameObjectChild = AddChild(gameObjectToDestroyLock);
	//this->gameObjectId = gameObjectToDestroyLock->GetUniqueId();
	//this->gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(gameObjectToDestroyLock->GetReflectiveData());
	//this->componentName = componentToDestroyLock->GetComponentName();
	//isEnabled = componentToDestroyLock->GetIsEnabled();
}

inline void InspectorDeleteGameObjectCommand::Execute()
{
	std::shared_ptr<GameObject> gameObjectToDestroy = FindGameObjectById(gameObjectChild.gameObjectId);
	if (gameObjectToDestroy)
	{
		Destroy(gameObjectToDestroy);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorDeleteGameObjectCommand::Undo()
{
	ReCreateChild(gameObjectChild, nullptr);
	UpdateChildComponents(gameObjectChild);
	/*std::shared_ptr<GameObject> gameObject = CreateGameObject();
	ReflectionUtils::JsonToReflectiveData(gameObjectData, gameObject->GetReflectiveData());
	gameObject->OnReflectionUpdated();
	gameObject->SetUniqueId(gameObjectId);
	SceneManager::SetSceneModified(true);*/
}