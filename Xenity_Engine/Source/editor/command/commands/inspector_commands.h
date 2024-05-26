// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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

template<typename T>
class ReflectiveChangeValueCommand : public Command
{
public:
	ReflectiveChangeValueCommand() = delete;
	ReflectiveChangeValueCommand(uint64_t targetId, int ownerType, const ReflectiveEntry& reflectiveEntry, T& newValue, T& lastValue);
	void Execute() override;
	void Undo() override;
private:
	void SetValue(const nlohmann::json& valueToSet, bool isUndo);

	uint64_t targetId = 0;
	int ownerType = -1;
	std::string variableName;
	nlohmann::json newValue;
	nlohmann::json lastValue;
};

template<typename T>
inline ReflectiveChangeValueCommand<T>::ReflectiveChangeValueCommand(uint64_t targetId, int ownerType, const ReflectiveEntry& reflectiveEntry, T& newValue, T& lastValue)
{
	this->targetId = targetId;
	this->ownerType = ownerType;
	
	variableName = reflectiveEntry.variableName;

	std::reference_wrapper<T> newValueRef = std::ref(newValue);
	std::reference_wrapper<T> lastValueRef = std::ref(lastValue);
	ReflectionUtils::VariableToJson(this->newValue, reflectiveEntry.variableName, &newValueRef);
	ReflectionUtils::VariableToJson(this->lastValue, reflectiveEntry.variableName, &lastValueRef);
	Debug::Print(this->newValue.dump(3));
	Debug::Print(this->lastValue.dump(3));
}

template<typename T>
inline void ReflectiveChangeValueCommand<T>::SetValue(const nlohmann::json& valueToSet, bool isUndo)
{
	bool hasBeenSet = false;
	if (targetId != 0)
	{
		if (ownerType == 0)
		{
			std::shared_ptr<FileReference> foundFileRef = ProjectManager::GetFileReferenceById(targetId);
			if (foundFileRef)
			{
				ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundFileRef->GetReflectiveData(), variableName));
				foundFileRef->OnReflectionUpdated();
				// Do not set scene as dirty
				hasBeenSet = true;
			}
		}
		else if (ownerType == 2)
		{
			std::shared_ptr<Component> foundComponent = FindComponentById(targetId);
			if (foundComponent)
			{
				ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundComponent->GetReflectiveData(), variableName));
				foundComponent->OnReflectionUpdated();
				SceneManager::SetSceneModified(true);
				hasBeenSet = true;
			}
		}
		else if (ownerType == 1)
		{
			std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
			if (foundGameObject)
			{
				ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundGameObject->GetReflectiveData(), variableName));
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

	if (hasBeenSet && isUndo)
	{
		Debug::Print("Undo value changed in Inspector");
	}
}

template<typename T>
inline void ReflectiveChangeValueCommand<T>::Execute()
{
	SetValue(newValue, false);
}

template<typename T>
inline void ReflectiveChangeValueCommand<T>::Undo()
{
	SetValue(lastValue, true);
}

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
				// Do not set scene as dirty
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

inline InspectorGameObjectSetActiveCommand::InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> _target, bool newValue, bool lastValue) : target(_target)
{
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

inline InspectorTransformSetLocalPositionCommand::InspectorTransformSetLocalPositionCommand(std::weak_ptr<Transform> _target, Vector3 newValue, Vector3 lastValue) : target(_target)
{
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

inline InspectorTransformSetLocalRotationCommand::InspectorTransformSetLocalRotationCommand(std::weak_ptr<Transform> _target, Vector3 newValue, Vector3 lastValue) : target(_target)
{
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

inline InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(std::weak_ptr<Transform> _target, Vector3 _newValue, Vector3 _lastValue) : target(_target), newValue(_newValue), lastValue(_lastValue)
{
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
	InspectorAddComponentCommand(std::weak_ptr<GameObject> target, const std::string& componentName);
	void Execute() override;
	void Undo() override;

	std::string componentName = "";
	uint64_t componentId = 0;
private:
	uint64_t targetId = 0;
};

inline InspectorAddComponentCommand::InspectorAddComponentCommand(std::weak_ptr<GameObject> target, const std::string& _componentName) : componentName(_componentName)
{
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
	InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& targets, int mode);
	void Execute() override;
	void Undo() override;
	std::vector<std::weak_ptr<GameObject>> createdGameObjects;
private:
	std::vector<std::weak_ptr<GameObject>> targets;
	std::vector<std::weak_ptr<GameObject>> oldParents;
	int mode; // 0 Create Empty, 1 Create Child, 2 Create parent
};

inline InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& _targets, int mode) : targets(_targets)
{
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

				if (target->GetParent().lock())
				{
					newGameObject->SetParent(target->GetParent().lock());
				}

				oldParents.push_back(target->GetParent().lock());

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
		Editor::SetSelectedFileReference(nullptr);
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
			const size_t targetCount = targets.size();
			for (size_t i = 0; i < targetCount; i++)
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
	std::shared_ptr<T> componentToDestroyLock = componentToDestroy.lock();
	this->target = componentToDestroyLock->GetGameObject();
	this->componentId = componentToDestroyLock->GetUniqueId();
	this->componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToDestroyLock->GetReflectiveData());
	this->componentName = componentToDestroyLock->GetComponentName();
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
		component->SetUniqueId(componentId);
		//componentId = component->GetUniqueId();
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
class InspectorSetComponentDataCommand : public Command
{
public:
	InspectorSetComponentDataCommand() = delete;
	InspectorSetComponentDataCommand(std::weak_ptr<T> componentToUse, nlohmann::json newComponentData);
	void Execute() override;
	void Undo() override;
private:
	uint64_t componentId = 0;
	nlohmann::json componentData;
	nlohmann::json oldComponentData;
	std::string componentName = "";
};

template<typename T>
inline InspectorSetComponentDataCommand<T>::InspectorSetComponentDataCommand(std::weak_ptr<T> componentToUse, nlohmann::json newComponentData) : componentData(newComponentData)
{
	this->componentId = componentToUse.lock()->GetUniqueId();
	this->oldComponentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.lock()->GetReflectiveData());
	this->componentName = componentToUse.lock()->GetComponentName();
}

template<typename T>
inline void InspectorSetComponentDataCommand<T>::Execute()
{
	std::shared_ptr<Component> componentToUpdate = FindComponentById(componentId);
	if (componentToUpdate)
	{
		ReflectionUtils::JsonToReflectiveData(componentData, componentToUpdate->GetReflectiveData());
		componentToUpdate->OnReflectionUpdated();
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
inline void InspectorSetComponentDataCommand<T>::Undo()
{
	std::shared_ptr<Component> componentToUpdate = FindComponentById(componentId);
	if (componentToUpdate)
	{
		ReflectionUtils::JsonToReflectiveData(oldComponentData, componentToUpdate->GetReflectiveData());
		componentToUpdate->OnReflectionUpdated();
		SceneManager::SetSceneModified(true);
	}
}

class InspectorSetTransformDataCommand : public Command
{
public:
	InspectorSetTransformDataCommand() = delete;
	InspectorSetTransformDataCommand(std::weak_ptr<Transform> componentToUse, nlohmann::json newComponentData);
	void Execute() override;
	void Undo() override;
private:
	uint64_t transformtId = 0;
	nlohmann::json transformData;
	nlohmann::json oldTransformData;
};

inline InspectorSetTransformDataCommand::InspectorSetTransformDataCommand(std::weak_ptr<Transform> componentToUse, nlohmann::json newTransformDataData) : transformData(newTransformDataData)
{
	this->transformtId = componentToUse.lock()->GetGameObject()->GetUniqueId();
	this->oldTransformData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.lock()->GetReflectiveData());
}

inline void InspectorSetTransformDataCommand::Execute()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(transformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorSetTransformDataCommand::Undo()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(oldTransformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetSceneModified(true);
	}
}