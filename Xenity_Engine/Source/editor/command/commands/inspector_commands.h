#pragma once

/**
 * [Internal]
 */

#include <memory>
#include <json.hpp>

#include <editor/command/command.h>
#include <editor/editor.h>

#include <engine/engine.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/asset_management/project_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/class_registry/class_registry.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/vectors/vector3.h>
#include <engine/component.h>
#include <engine/tools/gameplay_utility.h>

 //----------------------------------------------------------------------------

template<typename U, typename T>
class InspectorChangeValueCommand : public Command
{
public:
	InspectorChangeValueCommand() = delete;
	InspectorChangeValueCommand(std::weak_ptr<U> target, T* valuePtr, T newValue, T lastValue);
	void Execute() override;
	void Undo() override;
private:
	void SetValue(T valueToSet, bool isUndo);

	uint64_t targetId = 0;
	T* valuePtr;
	T newValue;
	T lastValue;
};

template<typename U, typename T>
inline InspectorChangeValueCommand<U, T>::InspectorChangeValueCommand(std::weak_ptr<U> target, T* valuePtr, T newValue, T lastValue)
{
	if constexpr (std::is_base_of<U, FileReference>())
	{
		if(target.lock())
			this->targetId = target.lock()->fileId;
	}
	else if constexpr (std::is_base_of<U, GameObject>() || std::is_base_of<U, Component>())
	{
		if(target.lock())
			this->targetId = target.lock()->GetUniqueId();
	}

	this->valuePtr = valuePtr;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::SetValue(T valueToSet, bool isUndo)
{
	bool hasBeenSet = false;
	if (targetId != 0)
	{
		if constexpr (std::is_base_of<U, FileReference>())
		{
			std::shared_ptr<FileReference> foundFileRef = ProjectManager::GetFileReferenceById(targetId);
			if (foundFileRef)
			{
				*valuePtr = valueToSet;
				foundFileRef->OnReflectionUpdated();
				hasBeenSet = true;
			}
		}
		else if constexpr (std::is_base_of<U, Component>())
		{
			std::shared_ptr<Component> foundComponent = FindComponentById(targetId);
			if (foundComponent)
			{
				*valuePtr = valueToSet;
				foundComponent->OnReflectionUpdated();
				SceneManager::SetSceneModified(true);
				hasBeenSet = true;
			}
		}
		else if constexpr (std::is_base_of<U, GameObject>())
		{
			std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
			if (foundGameObject)
			{
				*valuePtr = valueToSet;
				foundGameObject->OnReflectionUpdated();
				SceneManager::SetSceneModified(true);
				hasBeenSet = true;
			}
		}
		else
		{
			Debug::PrintError("Can't do Command!");
		}
	}
	else
	{
		*valuePtr = valueToSet;
		//hasBeenSet = true;
	}

	if (hasBeenSet && isUndo)
	{
		Debug::Print("Undo value changed in Inspector");
	}
}

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::Execute()
{
	SetValue(newValue, false);
}

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::Undo()
{
	SetValue(lastValue, true);
}


//----------------------------------------------------------------------------

class InspectorGameObjectSetActiveCommand : public Command
{
public:
	InspectorGameObjectSetActiveCommand() = delete;
	InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> target, bool newValue, bool lastValue);
	void Execute() override;
	void Undo() override;
private:
	std::weak_ptr<GameObject> target;
	bool newValue;
	bool lastValue;
};

inline InspectorGameObjectSetActiveCommand::InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> target, bool newValue, bool lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorGameObjectSetActiveCommand::Execute()
{
	if (target.lock())
	{
		target.lock()->SetActive(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorGameObjectSetActiveCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetActive(lastValue);
		SceneManager::SetSceneModified(true);
	}
}


//----------------------------------------------------------------------------

class InspectorTransformSetLocalPositionCommand : public Command
{
public:
	InspectorTransformSetLocalPositionCommand() = delete;
	InspectorTransformSetLocalPositionCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalPositionCommand::InspectorTransformSetLocalPositionCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalPositionCommand::Execute()
{
	if (target.lock())
	{
		target.lock()->SetLocalPosition(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalPositionCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalPosition(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetLocalRotationCommand : public Command
{
public:
	InspectorTransformSetLocalRotationCommand() = delete;
	InspectorTransformSetLocalRotationCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalRotationCommand::InspectorTransformSetLocalRotationCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalRotationCommand::Execute()
{
	if (target.lock())
	{
		target.lock()->SetLocalRotation(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalRotationCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalRotation(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetLocalScaleCommand : public Command
{
public:
	InspectorTransformSetLocalScaleCommand() = delete;
	InspectorTransformSetLocalScaleCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalScaleCommand::Execute()
{
	if (target.lock())
	{
		target.lock()->SetLocalScale(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalScaleCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalScale(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------


class InspectorAddComponentCommand : public Command
{
public:
	InspectorAddComponentCommand() = delete;
	InspectorAddComponentCommand(std::weak_ptr<GameObject> target, std::string componentName);
	void Execute() override;
	void Undo() override;

	std::string componentName = "";
	uint64_t componentId = 0;
private:
	uint64_t targetId = 0;
};

inline InspectorAddComponentCommand::InspectorAddComponentCommand(std::weak_ptr<GameObject> target, std::string componentName)
{
	this->componentName = componentName;
	if (target.lock())
		targetId = target.lock()->GetUniqueId();
}

inline void InspectorAddComponentCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentName, foundGameObject);
		if (newComponent) 
		{
			componentId = newComponent->GetUniqueId();
			SceneManager::SetSceneModified(true);
		}
		else 
		{
			componentId = 0;
		}
	}
}

inline void InspectorAddComponentCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject && componentId != 0)
	{
		std::shared_ptr<Component> oldComponent = FindComponentById(componentId);
		if (oldComponent) 
		{
			Destroy(oldComponent);
			SceneManager::SetSceneModified(true);
		}
	}
}


//----------------------------------------------------------------------------

class InspectorCreateGameObjectCommand : public Command
{
public:
	InspectorCreateGameObjectCommand() = delete;
	InspectorCreateGameObjectCommand(std::vector<std::weak_ptr<GameObject>> targets, int mode);
	void Execute() override;
	void Undo() override;
	std::vector<std::weak_ptr<GameObject>> createdGameObjects;
private:
	std::vector<std::weak_ptr<GameObject>> targets;
	std::vector<std::weak_ptr<GameObject>> oldParents;
	int mode; // 0 Create Empty, 1 Create Child, 2 Create parent
};

inline InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(std::vector<std::weak_ptr<GameObject>> targets, int mode)
{
	this->targets = targets;
	this->mode = mode;
}

inline void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	createdGameObjects.clear();
	oldParents.clear();

	if (mode == 0)
	{
		createdGameObjects.push_back(CreateGameObject());
		done = true;
	}
	else if (mode == 1)
	{
		for (std::weak_ptr<GameObject> weakTarget : targets)
		{
			if (auto target = weakTarget.lock())
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				createdGameObjects.push_back(newGameObject);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				newGameObject->SetParent(target);
				transform->SetLocalPosition(Vector3(0));
				transform->SetLocalRotation(Vector3(0));
				transform->SetLocalScale(Vector3(1));
				done = true;
			}
		}
	}
	else if (mode == 2)
	{
		for (std::weak_ptr<GameObject> weakTarget : targets)
		{
			if (auto target = weakTarget.lock())
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				createdGameObjects.push_back(newGameObject);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				std::shared_ptr<Transform> selectedTransform = target->GetTransform();
				transform->SetPosition(selectedTransform->GetPosition());
				transform->SetRotation(selectedTransform->GetRotation());
				transform->SetLocalScale(selectedTransform->GetScale());

				if (target->parent.lock())
				{
					newGameObject->SetParent(target->parent.lock());
				}

				oldParents.push_back(target->parent.lock());

				target->SetParent(newGameObject);
				done = true;
			}
		}
	}

	if (done)
	{
		Editor::ClearSelectedGameObjects();
		for (std::weak_ptr<GameObject> createdGameObject : createdGameObjects)
		{
			Editor::AddSelectedGameObject(createdGameObject.lock());
		}
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorCreateGameObjectCommand::Undo()
{
	if (createdGameObjects.size() > 0)
	{
		bool done = false;
		if (mode != 2)
		{
			for (std::weak_ptr<GameObject> weakGameObject : createdGameObjects)
			{
				Destroy(weakGameObject.lock());
			}
			done = true;
		}
		else
		{
			const int targetCount = targets.size();
			for (int i = 0; i < targetCount; i++)
			{
				if (oldParents[i].lock())
					targets[i].lock()->SetParent(oldParents[i].lock());
				else
					targets[i].lock()->SetParent(nullptr);

				Destroy(createdGameObjects[i].lock());
				done = true;
			}
		}

		if (done)
			SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

template<typename T>
class InspectorDeleteComponentCommand : public Command
{
public:
	InspectorDeleteComponentCommand() = delete;
	InspectorDeleteComponentCommand(std::weak_ptr<T> componentToDestroy);
	void Execute() override;
	void Undo() override;
private:
	std::weak_ptr<GameObject> target;
	uint64_t componentId = 0;
	nlohmann::json componentData;
	std::string componentName = "";
};

template<typename T>
inline InspectorDeleteComponentCommand<T>::InspectorDeleteComponentCommand(std::weak_ptr<T> componentToDestroy)
{
	this->target = componentToDestroy.lock()->GetGameObject();
	this->componentId = componentToDestroy.lock()->GetUniqueId();
	this->componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToDestroy.lock()->GetReflectiveData());
	this->componentName = componentToDestroy.lock()->GetComponentName();
}

template<typename T>
inline void InspectorDeleteComponentCommand<T>::Execute()
{
	std::shared_ptr<Component> componentToDestroy = FindComponentById(componentId);
	if (componentToDestroy)
	{
		Destroy(componentToDestroy);
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
inline void InspectorDeleteComponentCommand<T>::Undo()
{
	if (target.lock())
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(componentName, target.lock());
		ReflectionUtils::JsonToReflectiveData(componentData, component->GetReflectiveData());
		componentId = component->GetUniqueId();
		SceneManager::SetSceneModified(true);
	}
}