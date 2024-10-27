// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <engine/assertions/assertions.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/project_manager.h>

// List of all file types drawn by the EditorUI or the editor wont compile
#include <engine/graphics/skybox.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/icon.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/component.h>
#include <engine/physics/collider.h>
#include <engine/file_system/file_reference.h>
#include <engine/debug/stack_debug_object.h>

#pragma region Fill variables

// Template for basic types (int, float, strings...)
template<typename T>
std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
inline  ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<T> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	valuePtr.get() = jsonValue;
}

inline  void ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<Reflective> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	ReflectionUtils::JsonToReflective(jsonValue, valuePtr.get());
}

inline  void ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::vector<Reflective*>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const size_t jsonArraySize = jsonValue.size();
	const size_t objectVectorSize = valuePtr.get().size();

	for (size_t i = 0; i < jsonArraySize; i++)
	{
		Reflective* tempVariable = nullptr;
		if (jsonValue[i].contains("Values")) // If the reflective is not null
		{
			// Go through json Values list
			tempVariable = (Reflective*)entry.typeSpawner->Allocate();
			ReflectionUtils::JsonToReflective(jsonValue[i], *tempVariable);
		}
		if (i >= objectVectorSize) // If the vector is too small, add new slot
		{
			valuePtr.get().push_back(tempVariable);
		}
		else
		{
			valuePtr.get()[i] = tempVariable;
		}
	}
}

template<typename T>
std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value || std::is_same<T, uint64_t>::value
	|| std::is_same<T, double>::value || std::is_same<T, std::string>::value, void>
	inline ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::vector<T>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const size_t jsonArraySize = jsonValue.size();
	const size_t objectVectorSize = valuePtr.get().size();

	for (size_t i = 0; i < jsonArraySize; i++)
	{
		T tempVariable = jsonValue[i];
		if (i >= objectVectorSize)
		{
			valuePtr.get().push_back(tempVariable);
		}
		else
		{
			valuePtr.get()[i] = tempVariable;
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
inline ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::weak_ptr<T>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	if constexpr (std::is_same <T, GameObject>())
	{
		const auto go = FindGameObjectById(jsonValue);
		valuePtr.get() = go;
	}
	else if constexpr (std::is_same <T, Transform>())
	{
		const auto go = FindGameObjectById(jsonValue);
		if (go)
			valuePtr.get() = go->GetTransform();
		else
			valuePtr.get().reset();
	}
	else if constexpr (std::is_same <T, Component>())
	{
		const auto comp = FindComponentById(jsonValue);
		valuePtr.get() = comp;
	}
	else if constexpr (std::is_same <T, Collider>())
	{
		Debug::PrintError("[JsonToVariable] not implemented for std::weak_ptr<Collider>!", true);
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
inline ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::shared_ptr<T>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	ReflectionUtils::FillFileReference<T>(jsonValue, valuePtr);
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
inline ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::vector<std::weak_ptr<T>>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const size_t jsonArraySize = jsonValue.size();
	const size_t objectVectorSize = valuePtr.get().size();

	for (size_t i = 0; i < jsonArraySize; i++)
	{
		std::shared_ptr<T> tempVariable = nullptr;
		if (!jsonValue.at(i).is_null())
		{
			const uint64_t id = jsonValue.at(i);
			if constexpr (std::is_same <T, GameObject>())
			{
				tempVariable = FindGameObjectById(id);
			}
			else if constexpr (std::is_same <T, Transform>())
			{
				std::shared_ptr <GameObject> go = FindGameObjectById(id);
				if (go)
				{
					tempVariable = go->GetTransform();
				}
			}
			else if constexpr (std::is_same <T, Component>())
			{
				tempVariable = FindComponentById(id);
			}
			else if constexpr (std::is_same <T, Collider>())
			{
				Debug::PrintError("[JsonToVariable] not implemented for std::vector<std::weak_ptr<Collider>>!", true);
			}
		}
		if (i >= objectVectorSize)
		{
			valuePtr.get().push_back(tempVariable);
		}
		else
		{
			valuePtr.get()[i] = tempVariable;
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
inline ReflectionUtils::JsonToVariable(const nlohmann::ordered_json& jsonValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>> valuePtr, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	ReflectionUtils::FillVectorFileReference(jsonValue, valuePtr);
}

inline void ReflectionUtils::JsonToReflectiveData(const nlohmann::ordered_json& json, const ReflectiveData& dataList)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	if (json.contains("Values"))
	{
		// Go through json Values list
		for (const auto& kv : json["Values"].items())
		{
			// Check if the data list contains the variable name found in the json

			for (auto& otherEntry : dataList)
			{
				if (otherEntry.variableName == kv.key())
				{
					const VariableReference& variableRef = otherEntry.variable.value();
					const auto& kvValue = kv.value();
					std::visit([&kvValue, &otherEntry](const auto& value)
						{
							JsonToVariable(kvValue, value, otherEntry);
						}, variableRef);
					break;
				}
			}
		}
	}
}

inline void ReflectionUtils::JsonToReflectiveEntry(const nlohmann::ordered_json& json, const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const VariableReference& variableRef = entry.variable.value();
	if (json.is_null())
	{
		const auto& kvValue = 0;
		std::visit([&kvValue, &entry](const auto& value)
			{
				JsonToVariable(kvValue, value, entry);
			}, variableRef);
	}
	else
	{
		const auto& kvValue = json.at(entry.variableName);
		std::visit([&kvValue, &entry](const auto& value)
			{
				JsonToVariable(kvValue, value, entry);
			}, variableRef);
	}
}

inline ReflectiveEntry ReflectionUtils::GetReflectiveEntryByName(const ReflectiveData& dataList, const std::string& name)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	for (const ReflectiveEntry& entry : dataList)
	{
		if (entry.variableName == name)
		{
			return entry;
		}
	}

	return ReflectiveEntry();
}

inline void ReflectionUtils::ReflectiveToReflective(Reflective& fromReflective, Reflective& toReflective)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const ReflectiveData fromReflectiveData = fromReflective.GetReflectiveData();
	nlohmann::ordered_json jsonData;
	jsonData["Values"] = ReflectiveDataToJson(fromReflectiveData);

	ReflectiveData toReflectiveData = toReflective.GetReflectiveData();
	JsonToReflectiveData(jsonData, toReflectiveData);
	toReflective.OnReflectionUpdated();
}

inline void ReflectionUtils::JsonToReflective(const nlohmann::ordered_json& j, Reflective& reflective)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const ReflectiveData myMap = reflective.GetReflectiveData();
	JsonToReflectiveData(j, myMap);
	reflective.OnReflectionUpdated();
}

#pragma endregion

#pragma region Fill json

// Template for basic types (int, float, strings...)
template<typename T>
std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
inline ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<T> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson0] key is empty");

	jsonValue[key] = valuePtr.get();
}

inline void ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<Reflective> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson1] key is empty");

	jsonValue[key]["Values"] = ReflectionUtils::ReflectiveToJson(valuePtr.get());
}

inline void ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<Reflective*>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson2] key is empty");

	std::vector <Reflective*>& getVal = valuePtr.get();
	const size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		if (!getVal[vIndex])
			jsonValue[key][vIndex]["Values"] = nullptr;
		else
			jsonValue[key][vIndex]["Values"] = ReflectionUtils::ReflectiveToJson(*getVal[vIndex]);
	}
}

template<typename T>
std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value || std::is_same<T, uint64_t>::value
	|| std::is_same<T, double>::value || std::is_same<T, std::string>::value, void>
	inline 	ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<T>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson3] key is empty");

	std::vector <T>& getVal = valuePtr.get();
	const size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		jsonValue[key][vIndex] = getVal[vIndex];
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
inline ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::weak_ptr<T>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson4] key is empty");

	if (const auto lockValue = (valuePtr.get()).lock())
	{
		if constexpr (std::is_same <T, GameObject>())
		{
			jsonValue[key] = lockValue->GetUniqueId();
		}
		else if constexpr (std::is_same <T, Transform>())
		{
			jsonValue[key] = lockValue->GetGameObject()->GetUniqueId();
		}
		else if constexpr (std::is_same <T, Component>())
		{
			jsonValue[key] = lockValue->GetUniqueId();
		}
		else if constexpr (std::is_same <T, Collider>())
		{
			Debug::PrintError("[VariableToJson] not implemented for std::weak_ptr<Collider>!", true);
		}
	}
	else
	{
		jsonValue[key] = 0;
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
inline ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::shared_ptr<T>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson5] key is empty");

	if (valuePtr.get() != nullptr)
		jsonValue[key] = valuePtr.get()->GetFileId();
	else
		jsonValue[key] = 0;
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
inline ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::weak_ptr<T>>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson6] key is empty");

	const std::vector <std::weak_ptr<T>>& getVal = valuePtr.get();
	const size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		if (getVal.at(vIndex).lock())
		{
			if constexpr (std::is_same <T, GameObject>())
			{
				jsonValue[key][vIndex] = getVal.at(vIndex).lock()->GetUniqueId();
			}
			else if constexpr (std::is_same <T, Transform>())
			{
				jsonValue[key][vIndex] = getVal.at(vIndex).lock()->GetGameObject()->GetUniqueId();
			}
			else if constexpr (std::is_same <T, Component>())
			{
				jsonValue[key][vIndex] = getVal.at(vIndex).lock()->GetUniqueId();
			}
			else if constexpr (std::is_same <T, Collider>())
			{
				Debug::PrintError("[VariableToJson] not implemented for std::vector<std::weak_ptr<Collider>>!", true);
			}
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
inline ReflectionUtils::VariableToJson(nlohmann::ordered_json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::shared_ptr<T>>> valuePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(!key.empty(), "[ReflectionUtils::VariableToJson7] key is empty");

	const std::vector <std::shared_ptr<T>>& getVal = valuePtr.get();
	const size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		if (getVal.at(vIndex))
			jsonValue[key][vIndex] = getVal.at(vIndex)->GetFileId();
		else
			jsonValue[key][vIndex] = 0;
	}
}

inline nlohmann::ordered_json ReflectionUtils::ReflectiveDataToJson(const ReflectiveData& dataList)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	nlohmann::ordered_json json;
	for (const ReflectiveEntry& entry : dataList)
	{
		const VariableReference& variableRef = entry.variable.value();

		std::visit([&entry, &json](const auto& value)
			{
				VariableToJson(json, entry.variableName, value);
			}, variableRef);
	}
	return json;
}

inline nlohmann::ordered_json ReflectionUtils::ReflectiveToJson(Reflective& reflective)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const auto dataList = reflective.GetReflectiveData();
	const nlohmann::ordered_json jsonData = ReflectiveDataToJson(dataList);
	return jsonData;
}

inline nlohmann::ordered_json ReflectionUtils::ReflectiveEntryToJson(const ReflectiveEntry& entry)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	nlohmann::ordered_json json;
	std::visit([&entry, &json](const auto& value)
		{
			VariableToJson(json, entry.variableName, value);
		}, entry.variable.value());

	return json;
}

template <typename T>
inline void ReflectionUtils::FillFileReference(const uint64_t fileId, const std::reference_wrapper<std::shared_ptr<T>> variablePtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	if (fileId == 0)
	{
		variablePtr.get() = nullptr;
	}

	std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId); // Try to find the file reference
	if (file)
	{
		// Load file data
		file->LoadFileReference();
		//Put the file in the variable reference
		variablePtr.get() = std::dynamic_pointer_cast<T>(file);
	}
}

template <typename T>
inline void ReflectionUtils::FillVectorFileReference(const nlohmann::ordered_json& jsonVectorData, const std::reference_wrapper<std::vector<std::shared_ptr<T>>> vectorRefPtr)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	const size_t jsonArraySize = jsonVectorData.size();
	const size_t vectorSize = vectorRefPtr.get().size();

	for (size_t i = 0; i < jsonArraySize; i++)
	{
		std::shared_ptr<FileReference> file = nullptr;
		if (!jsonVectorData.at(i).is_null())
		{
			const uint64_t fileId = jsonVectorData.at(i);
			file = ProjectManager::GetFileReferenceById(fileId);
			if (file)
				file->LoadFileReference();
		}

		// Add the file to the vector
		if (i >= vectorSize)
		{
			vectorRefPtr.get().push_back(std::dynamic_pointer_cast<T>(file));
		}
		else
		{
			vectorRefPtr.get()[i] = std::dynamic_pointer_cast<T>(file);
		}
	}
}

#pragma endregion

#pragma region IO

inline bool ReflectionUtils::FileToReflectiveData(const std::shared_ptr<File>& file, const ReflectiveData& dataList)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(file != nullptr, "[ReflectionUtils::FileToReflectiveData] file is nullptr");

	bool ok = false;

	if (file->Open(FileMode::ReadOnly))
	{
		const std::string jsonString = file->ReadAll();
		file->Close();
		if (!jsonString.empty())
		{
			try
			{
				nlohmann::ordered_json myJson = nlohmann::ordered_json::parse(jsonString);
				ReflectionUtils::JsonToReflectiveData(myJson, dataList);
				ok = true;
			}
			catch (const std::exception&)
			{
				ok = false;
			}
		}
		else
		{
			ok = false;
		}
	}
	else
	{
		ok = false;
	}

	return ok;
}

inline bool ReflectionUtils::ReflectiveDataToFile(const ReflectiveData& dataList, const std::shared_ptr<File>& file)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(file != nullptr, "[ReflectionUtils::ReflectiveDataToFile] file is nullptr");

	bool ok = false;
	nlohmann::ordered_json myJson;
	myJson["Values"] = ReflectionUtils::ReflectiveDataToJson(dataList);
	ok = JsonToFile(myJson, file);

	return ok;
}

inline bool ReflectionUtils::JsonToFile(const nlohmann::ordered_json& data, std::shared_ptr<File> file)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	XASSERT(file != nullptr, "[ReflectionUtils::JsonToFile] file is nullptr");

	bool ok = false;
	FileSystem::s_fileSystem->Delete(file->GetPath());
	if (file->Open(FileMode::WriteCreateFile))
	{
		file->Write(data.dump(0));
		file->Close();
		ok = true;
	}
	else
	{
		ok = false;
	}

	return ok;
}

#pragma endregion