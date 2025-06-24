// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
* IMPORTANT: Do not store pointers to GameObjects, Components, Transforms, etc. in commands.
* This is because the pointers can become invalid if the object is deleted. Use the unique id instead.
*/

#include <memory>
#include <json.hpp>

#include <editor/command/command.h>
#include <editor/ui/reflective_data_to_draw.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/scene_management/scene_manager.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

template<typename T>
class ReflectiveChangeValueCommand : public Command
{
public:
	ReflectiveChangeValueCommand() = delete;
	ReflectiveChangeValueCommand(ReflectiveDataToDraw& reflectiveDataToDraw, T* valuePtr, T& oldValue, T& newValue);
	void Execute() override;
	void Undo() override;
private:
	void SetValue(const nlohmann::json& valueToSet, bool isUndo);
	void FindValueToChange(ReflectiveDataToDraw& reflectiveDataToDraw, nlohmann::json& jsonToChange, int currentIndex);

	uint64_t targetId = 0;
	ReflectiveDataToDraw::OwnerTypeEnum ownerType = ReflectiveDataToDraw::OwnerTypeEnum::None;
	std::string variableName;
	T* valuePtr;
	ReflectiveEntry reflectiveEntry;
	nlohmann::json newValue2;
	nlohmann::json lastValue2;
	bool isMetadata = false;
	AssetPlatform platform;
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
inline ReflectiveChangeValueCommand<T>::ReflectiveChangeValueCommand(ReflectiveDataToDraw& reflectiveDataToDraw, T* valuePtr, T& oldValue, T& newValue)
{
	this->valuePtr = valuePtr;
	targetId = reflectiveDataToDraw.ownerUniqueId;
	ownerType = reflectiveDataToDraw.ownerType;

	variableName = reflectiveDataToDraw.currentEntry.variableName;
	reflectiveEntry = reflectiveDataToDraw.currentEntry;
	isMetadata = reflectiveDataToDraw.isMeta;
	platform = reflectiveDataToDraw.platform;

	nlohmann::ordered_json newValueTemp;
	nlohmann::ordered_json lastValueTemp;

	// Ugly code
	// Save variable alone to json (new and old values)
	ReflectionUtils::VariableToJson(newValueTemp, reflectiveDataToDraw.currentEntry.variableName, std::ref(newValue));
	ReflectionUtils::VariableToJson(lastValueTemp, reflectiveDataToDraw.currentEntry.variableName, std::ref(oldValue));

	// Save the whole reflective data to json (new and old values) for later use
	for (const auto& kv : lastValueTemp.items())
	{
		ReflectionUtils::JsonToVariable(kv.value(), std::ref(*valuePtr), reflectiveDataToDraw.currentEntry);
	}
	lastValue2["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectiveDataToDraw.reflectiveDataStack[0]);
	for (const auto& kv : newValueTemp.items())
	{
		ReflectionUtils::JsonToVariable(kv.value(), std::ref(*valuePtr), reflectiveDataToDraw.currentEntry);
	}
	newValue2["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectiveDataToDraw.reflectiveDataStack[0]);
	for (const auto& kv : lastValueTemp.items())
	{
		ReflectionUtils::JsonToVariable(kv.value(), std::ref(*valuePtr), reflectiveDataToDraw.currentEntry);
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
	Debug::Print("lastValue\n" + this->lastValue.dump(3));*/
	//Debug::Print("lastValue2\n" + lastValue2.dump(3));
	//Debug::Print("newValue2\n" + newValue2.dump(3));
}

template<typename T>
inline void ReflectiveChangeValueCommand<T>::SetValue(const nlohmann::json& valueToSet, bool isUndo)
{
	bool hasBeenSet = false;
	if (targetId != 0)
	{
		if (ownerType == ReflectiveDataToDraw::OwnerTypeEnum::FileReference)
		{
			std::shared_ptr<FileReference> foundFileRef = ProjectManager::GetFileReferenceById(targetId);
			if (foundFileRef)
			{
				if(isMetadata)
					ReflectionUtils::JsonToReflectiveData(valueToSet, foundFileRef->GetMetaReflectiveData(platform));
				else
					ReflectionUtils::JsonToReflectiveData(valueToSet, foundFileRef->GetReflectiveData());
				foundFileRef->OnReflectionUpdated();
				// Do not set scene as dirty
				hasBeenSet = true;
			}
		}
		else if (ownerType == ReflectiveDataToDraw::OwnerTypeEnum::Component)
		{
			std::shared_ptr<Component> foundComponent = FindComponentById(targetId);
			if (foundComponent)
			{
				ReflectionUtils::JsonToReflectiveData(valueToSet, foundComponent->GetReflectiveData());
				foundComponent->OnReflectionUpdated();
				SceneManager::SetIsSceneDirty(true);
				hasBeenSet = true;
			}
		}
		else if (ownerType == ReflectiveDataToDraw::OwnerTypeEnum::GameObject)
		{
			std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
			if (foundGameObject)
			{
				ReflectionUtils::JsonToReflectiveData(valueToSet, foundGameObject->GetReflectiveData());
				foundGameObject->OnReflectionUpdated();
				SceneManager::SetIsSceneDirty(true);
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
		ReflectionUtils::JsonToVariable(valueToSet["Values"][variableName], std::ref(*valuePtr), reflectiveEntry);
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
//----------------------------------------------------------------------------

template<typename U, typename T>
class InspectorChangeValueCommand : public Command
{
public:
	InspectorChangeValueCommand() = delete;
	InspectorChangeValueCommand(const std::weak_ptr<U>& target, T* valuePtr, T newValue, T lastValue);
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
inline InspectorChangeValueCommand<U, T>::InspectorChangeValueCommand(const std::weak_ptr<U>& target, T* valuePtr, T newValue, T lastValue)
{
	if constexpr (std::is_base_of<U, FileReference>())
	{
		if (target.lock())
			this->targetId = target.lock()->m_fileId;
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
				SceneManager::SetIsSceneDirty(true);
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
				SceneManager::SetIsSceneDirty(true);
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
//----------------------------------------------------------------------------

template<typename T>
class InspectorItemSetActiveCommand : public Command
{
public:
	InspectorItemSetActiveCommand() = delete;
	InspectorItemSetActiveCommand(const T& target, bool newValue);
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
			SceneManager::SetIsSceneDirty(true);
		}
	}
	else if constexpr (std::is_base_of<T, GameObject>())
	{
		std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
		if (foundGameObject)
		{
			foundGameObject->SetActive(valueToSet);
			foundGameObject->OnReflectionUpdated();
			SceneManager::SetIsSceneDirty(true);
		}
	}
	else
	{
		Debug::PrintError("Can't do Command!");
	}
}

template<typename T>
inline InspectorItemSetActiveCommand<T>::InspectorItemSetActiveCommand(const T& target, bool newValue)
{
	if constexpr (std::is_base_of<T, GameObject>() || std::is_base_of<T, Component>())
	{
		this->targetId = target.GetUniqueId();
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
//----------------------------------------------------------------------------

template<typename T>
class InspectorItemSetStaticCommand : public Command
{
public:
	InspectorItemSetStaticCommand() = delete;
	InspectorItemSetStaticCommand(const T& target, bool newValue);
	void Execute() override;
	void Undo() override;
private:
	void ApplyValue(bool valueToSet);
	uint64_t targetId = 0;
	bool newValue;
};

template<typename T>
inline void InspectorItemSetStaticCommand<T>::ApplyValue(bool valueToSet)
{
	if constexpr (std::is_base_of<T, GameObject>())
	{
		std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
		if (foundGameObject)
		{
			foundGameObject->m_isStatic = valueToSet;
			foundGameObject->OnReflectionUpdated();
			SceneManager::SetIsSceneDirty(true);
		}
	}
	else
	{
		Debug::PrintError("Can't do Command!");
	}
}

template<typename T>
inline InspectorItemSetStaticCommand<T>::InspectorItemSetStaticCommand(const T& target, bool newValue)
{
	if constexpr (std::is_base_of<T, GameObject>() || std::is_base_of<T, Component>())
	{
		this->targetId = target.GetUniqueId();
	}
	this->newValue = newValue;
}

template<typename T>
inline void InspectorItemSetStaticCommand<T>::Execute()
{
	ApplyValue(newValue);
}

template<typename T>
inline void InspectorItemSetStaticCommand<T>::Undo()
{
	ApplyValue(!newValue);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorRectTransformSetPositionCommand : public Command
{
public:
	InspectorRectTransformSetPositionCommand() = delete;
	InspectorRectTransformSetPositionCommand(uint64_t _targetId, const Vector2& newValue, const Vector2& lastValue);
	void Execute() override;
	void Undo() override;
private:
	uint64_t m_targetId;
	Vector2 m_newValue;
	Vector2 m_lastValue;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorTransformSetPositionCommand : public Command
{
public:
	InspectorTransformSetPositionCommand() = delete;
	InspectorTransformSetPositionCommand(uint64_t _targetId, const Vector3& newValue, const Vector3& lastValue, bool isLocalPosition);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
	bool isLocalPosition;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorTransformSetRotationCommand : public Command
{
public:
	InspectorTransformSetRotationCommand() = delete;
	InspectorTransformSetRotationCommand(uint64_t _targetId, const Vector3& newValue, const Vector3& lastValue, bool isLocalRotation);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
	bool isLocalRotation;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorTransformSetLocalScaleCommand : public Command
{
public:
	InspectorTransformSetLocalScaleCommand() = delete;
	InspectorTransformSetLocalScaleCommand(uint64_t _target, const Vector3& newValue, const Vector3& lastValue);
	void Execute() override;
	void Undo() override;
private:
	uint64_t targetId;
	Vector3 newValue;
	Vector3 lastValue;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

template<typename T>
class InspectorSetComponentDataCommand : public Command
{
public:
	InspectorSetComponentDataCommand() = delete;
	InspectorSetComponentDataCommand(T& componentToUse, const nlohmann::json& newComponentData);
	void Execute() override;
	void Undo() override;
private:
	uint64_t componentId = 0;
	nlohmann::json componentData;
	nlohmann::json oldComponentData;
	std::string componentName = "";
};

template<typename T>
inline InspectorSetComponentDataCommand<T>::InspectorSetComponentDataCommand(T& componentToUse, const nlohmann::json& newComponentData) : componentData(newComponentData)
{
	this->componentId = componentToUse.GetUniqueId();
	this->oldComponentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.GetReflectiveData());
	this->componentName = componentToUse.GetComponentName();
}

template<typename T>
inline void InspectorSetComponentDataCommand<T>::Execute()
{
	std::shared_ptr<Component> componentToUpdate = FindComponentById(componentId);
	if (componentToUpdate)
	{
		ReflectionUtils::JsonToReflectiveData(componentData, componentToUpdate->GetReflectiveData());
		componentToUpdate->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
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
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorSetTransformDataCommand : public Command
{
public:
	InspectorSetTransformDataCommand() = delete;
	InspectorSetTransformDataCommand(Transform& transform, const nlohmann::json& newComponentData);
	void Execute() override;
	void Undo() override;
private:
	uint64_t transformtId = 0;
	nlohmann::json transformData;
	nlohmann::json oldTransformData;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------