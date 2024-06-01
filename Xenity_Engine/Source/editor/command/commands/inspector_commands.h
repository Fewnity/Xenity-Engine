// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
* IMPORTANT: Do not store pointers to GameObjects, Components, Transforms, etc. in commands.
* This is because the pointers can become invalid if the object is deleted. Use the unique id instead.
*/

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
#include <editor/ui/reflective_data_to_draw.h>

 //----------------------------------------------------------------------------

template<typename T>
class ReflectiveChangeValueCommand : public Command
{
public:
	ReflectiveChangeValueCommand() = delete;
	ReflectiveChangeValueCommand(ReflectiveDataToDraw& reflectiveDataToDraw, uint64_t targetId, int ownerType, const ReflectiveEntry& reflectiveEntry, T* valuePtr, T& newValue, T& lastValue);
	void Execute() override;
	void Undo() override;
private:
	void SetValue(const nlohmann::json& valueToSet, bool isUndo);
	void FindValueToChange(ReflectiveDataToDraw& reflectiveDataToDraw, nlohmann::json& jsonToChange, int currentIndex);

	uint64_t targetId = 0;
	int ownerType = -1;
	std::string variableName;

	nlohmann::json newValue2;
	nlohmann::json lastValue2;
};

template<typename T>
void ReflectiveChangeValueCommand<T>::FindValueToChange(ReflectiveDataToDraw& reflectiveDataToDraw, nlohmann::json& jsonToChange, int currentIndex)
{
	/*if (json.is_object())
	{
		for (auto it = json.begin(); it != json.end(); ++it)
		{
			if (it.key() == key)
			{
				json.erase(it);
				break;
			}
			else
			{
				RecusivelyErase(reflectiveDataToDraw, it.value(), key);
			}
		}
	}*/
}

template<typename T>
inline ReflectiveChangeValueCommand<T>::ReflectiveChangeValueCommand(ReflectiveDataToDraw& reflectiveDataToDraw, uint64_t targetId, int ownerType, const ReflectiveEntry& reflectiveEntry, T* valuePtr, T& newValue, T& lastValue)
{
	this->targetId = targetId;
	this->ownerType = ownerType;

	variableName = reflectiveEntry.variableName;


	nlohmann::json newValueTemp;
	nlohmann::json lastValueTemp;

	// Ugly code
	// Save variable alone to json (new and old values)
	ReflectionUtils::VariableToJson(newValueTemp, reflectiveEntry.variableName, std::ref(newValue));
	ReflectionUtils::VariableToJson(lastValueTemp, reflectiveEntry.variableName, std::ref(lastValue));

	// Save the whole reflective data to json (new and old values) for later use
	lastValue2["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectiveDataToDraw.reflectiveDataStack[0]);
	for (const auto& kv : newValueTemp.items())
	{
		ReflectionUtils::JsonToVariable(kv.value(), std::ref(*valuePtr), reflectiveEntry);
	}
	newValue2["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectiveDataToDraw.reflectiveDataStack[0]);
	for (const auto& kv : lastValueTemp.items())
	{
		ReflectionUtils::JsonToVariable(kv.value(), std::ref(*valuePtr), reflectiveEntry);
	}

	// Remove all other variables from the json
	//int itemCount = temp.items().begin().key().size();
	////temp.erase(reflectiveEntry.variableName);
	//auto items = temp.items();
	//for (auto& j : items)
	////for (int i = 0; i < itemCount; i++)
	//{
	//	//auto j = temp.items;
	//	std::string k = j.key();
	//	if (k != reflectiveDataToDraw.entryStack[0].variableName)
	//	{
	//		temp2.erase(k); 
	//	}
	//	//items = temp.items();
	//}

	/*Debug::Print("newValue\n" + this->newValue.dump(3));
	Debug::Print("lastValue\n" + this->lastValue.dump(3));
	Debug::Print("lastValue2\n" + lastValue2.dump(3));
	Debug::Print("newValue2\n" + newValue2.dump(3));*/
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
				ReflectionUtils::JsonToReflectiveData(valueToSet, foundFileRef->GetReflectiveData());
				//ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundFileRef->GetReflectiveData(), variableName));
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
				ReflectionUtils::JsonToReflectiveData(valueToSet, foundComponent->GetReflectiveData());
				//ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundComponent->GetReflectiveData(), variableName));
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
				ReflectionUtils::JsonToReflectiveData(valueToSet, foundGameObject->GetReflectiveData());
				//ReflectionUtils::JsonToReflectiveEntry(valueToSet, ReflectionUtils::GetReflectiveEntryByName(foundGameObject->GetReflectiveData(), variableName));
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
	SetValue(newValue2, false);
}

template<typename T>
inline void ReflectiveChangeValueCommand<T>::Undo()
{
	SetValue(lastValue2, true);
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
		if (target.lock())
			this->targetId = target.lock()->fileId;
	}
	else if constexpr (std::is_base_of<U, GameObject>() || std::is_base_of<U, Component>())
	{
		if (target.lock())
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
template<typename T>
class InspectorItemSetActiveCommand : public Command
{
public:
	InspectorItemSetActiveCommand() = delete;
	InspectorItemSetActiveCommand(std::shared_ptr<T> target, bool newValue);
	void Execute() override;
	void Undo() override;
private:
	void ApplyValue(bool valueToSet);
	uint64_t targetId = 0;
	bool newValue;
};

template<typename T>
inline void InspectorItemSetActiveCommand<T>::ApplyValue(bool valueToSet)
{
	if constexpr (std::is_base_of<T, Component>())
	{
		std::shared_ptr<Component> foundComponent = FindComponentById(targetId);
		if (foundComponent)
		{
			foundComponent->SetIsEnabled(valueToSet);
			foundComponent->OnReflectionUpdated();
			SceneManager::SetSceneModified(true);
		}
	}
	else if constexpr (std::is_base_of<T, GameObject>())
	{
		std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
		if (foundGameObject)
		{
			foundGameObject->SetActive(valueToSet);
			foundGameObject->OnReflectionUpdated();
			SceneManager::SetSceneModified(true);
		}
	}
	else
	{
		Debug::PrintError("Can't do Command!");
	}
}

template<typename T>
inline InspectorItemSetActiveCommand<T>::InspectorItemSetActiveCommand(std::shared_ptr<T> target, bool newValue)
{
	if constexpr (std::is_base_of<T, GameObject>() || std::is_base_of<T, Component>())
	{
		if (target)
			this->targetId = target->GetUniqueId();
	}
	this->newValue = newValue;
}

template<typename T>
inline void InspectorItemSetActiveCommand<T>::Execute()
{
	ApplyValue(newValue);
}

template<typename T>
inline void InspectorItemSetActiveCommand<T>::Undo()
{
	ApplyValue(!newValue);
}


//----------------------------------------------------------------------------

class InspectorTransformSetPositionCommand : public Command
{
public:
	InspectorTransformSetPositionCommand() = delete;
	InspectorTransformSetPositionCommand(uint64_t _targetId, Vector3 newValue, Vector3 lastValue, bool isLocalPosition);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
	bool isLocalPosition;
};

inline InspectorTransformSetPositionCommand::InspectorTransformSetPositionCommand(uint64_t _targetId, Vector3 newValue, Vector3 lastValue, bool isLocalPosition)
{
	this->targetId = _targetId;
	this->newValue = newValue;
	this->lastValue = lastValue;
	this->isLocalPosition = isLocalPosition;
}

inline void InspectorTransformSetPositionCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if(isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(newValue);
		else
			foundGameObject->GetTransform()->SetPosition(newValue);

		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetPositionCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if(isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(lastValue);
		else
			foundGameObject->GetTransform()->SetPosition(lastValue);

		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetRotationCommand : public Command
{
public:
	InspectorTransformSetRotationCommand() = delete;
	InspectorTransformSetRotationCommand(uint64_t _targetId, Vector3 newValue, Vector3 lastValue, bool isLocalRotation);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
	bool isLocalRotation;
};

inline InspectorTransformSetRotationCommand::InspectorTransformSetRotationCommand(uint64_t _targetId, Vector3 newValue, Vector3 lastValue, bool isLocalRotation)
{
	this->targetId = _targetId;
	this->newValue = newValue;
	this->lastValue = lastValue;
	this->isLocalRotation = isLocalRotation;
}

inline void InspectorTransformSetRotationCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if(isLocalRotation)
			foundGameObject->GetTransform()->SetLocalRotation(newValue);
		else
			foundGameObject->GetTransform()->SetRotation(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetRotationCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if(isLocalRotation)
			foundGameObject->GetTransform()->SetLocalRotation(lastValue);
		else
			foundGameObject->GetTransform()->SetRotation(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetLocalScaleCommand : public Command
{
public:
	InspectorTransformSetLocalScaleCommand() = delete;
	InspectorTransformSetLocalScaleCommand(uint64_t _target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(uint64_t _targetId, Vector3 _newValue, Vector3 _lastValue) : newValue(_newValue), lastValue(_lastValue)
{
	this->targetId = _targetId;
}

inline void InspectorTransformSetLocalScaleCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(newValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalScaleCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(lastValue);
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
	std::vector<uint64_t> createdGameObjects;
private:
	std::vector<uint64_t> targets;
	std::vector<uint64_t> oldParents;
	int mode; // 0 Create Empty, 1 Create Child, 2 Create parent
	bool alreadyExecuted = false;
};

inline InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& _targets, int mode)// : targets(_targets)
{
	this->mode = mode;

	for (std::weak_ptr<GameObject> weakTarget : _targets)
	{
		if (auto target = weakTarget.lock())
		{
			targets.push_back(target->GetUniqueId());
		}
	}
}

inline void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	if (mode == 0)
	{
		std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		if (!alreadyExecuted)
			createdGameObjects.push_back(newGameObject->GetUniqueId());
		else
			newGameObject->SetUniqueId(createdGameObjects[0]);
		done = true;
	}
	else if (mode == 1)
	{
		const size_t targetCount = targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
			if(target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
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
		const size_t targetCount = targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
			if (target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				std::shared_ptr<Transform> selectedTransform = target->GetTransform();
				transform->SetPosition(selectedTransform->GetPosition());
				transform->SetRotation(selectedTransform->GetRotation());
				transform->SetLocalScale(selectedTransform->GetScale());

				if (target->GetParent().lock())
				{
					newGameObject->SetParent(target->GetParent().lock());
				}
				if(target->GetParent().lock())
					oldParents.push_back(target->GetParent().lock()->GetUniqueId());
				else
					oldParents.push_back(0);

				target->SetParent(newGameObject);
				done = true;
			}
		}
	}

	alreadyExecuted = true;

	if (done)
	{
		Editor::ClearSelectedGameObjects();
		for (uint64_t createdGameObjectId : createdGameObjects)
		{
			std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(createdGameObjectId);
			if(createdGameObject)
				Editor::AddSelectedGameObject(createdGameObject);
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
			for (uint64_t gameObjectId : createdGameObjects)
			{
				std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(gameObjectId);
				Destroy(createdGameObject);
			}
			done = true;
		}
		else
		{
			const size_t targetCount = targets.size();
			for (size_t i = 0; i < targetCount; i++)
			{
				std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
				if (oldParents[i] != 0) 
				{
					std::shared_ptr<GameObject> oldParent = FindGameObjectById(oldParents[i]);
					target->SetParent(oldParent);
				}
				else
					target->SetParent(nullptr);

				std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(createdGameObjects[i]);
				Destroy(createdGameObject);
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
	uint64_t gameObjectId = 0;
	uint64_t componentId = 0;
	nlohmann::json componentData;
	std::string componentName = "";
};

template<typename T>
inline InspectorDeleteComponentCommand<T>::InspectorDeleteComponentCommand(std::weak_ptr<T> componentToDestroy)
{
	std::shared_ptr<T> componentToDestroyLock = componentToDestroy.lock();
	this->componentId = componentToDestroyLock->GetUniqueId();
	this->gameObjectId = componentToDestroyLock->GetGameObject()->GetUniqueId();
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
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(gameObjectId);
	if (gameObject)
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(componentName, gameObject);
		ReflectionUtils::JsonToReflectiveData(componentData, component->GetReflectiveData());
		component->SetUniqueId(componentId);
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