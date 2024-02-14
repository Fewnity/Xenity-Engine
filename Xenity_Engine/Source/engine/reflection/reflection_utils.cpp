#include "reflection_utils.h"

#include <engine/asset_management/project_manager.h>
#include <engine/scene_management/scene.h>

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_reference.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/component.h>
#include <engine/physics/collider.h>

#include <engine/graphics/skybox.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/ui/font.h>

#include <engine/audio/audio_clip.h>
#include <engine/debug/debug.h>

using json = nlohmann::json;

#pragma region Fill variables

// Template for basic types (int, float, strings...)
template<typename T>
std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<T>* valuePtr)
{
	valuePtr->get() = jsonValue;
}

void ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<Reflective>* valuePtr)
{
	ReflectionUtils::JsonToReflective(jsonValue, valuePtr->get());
}

void ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<std::vector<Reflective>>* valuePtr)
{
	Debug::PrintError("[JsonToVariable] not implemented for std::vector<Reflective>!");
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr)
{
	if constexpr (std::is_same <T, GameObject>())
	{
		auto go = FindGameObjectById(jsonValue);
		valuePtr->get() = go;
	}
	else if constexpr (std::is_same <T, Transform>())
	{
		auto go = FindGameObjectById(jsonValue);
		valuePtr->get() = go->GetTransform();
	}
	else if constexpr (std::is_same <T, Component>())
	{
		auto comp = FindComponentById(jsonValue);
		valuePtr->get() = comp;
	}
	else if constexpr (std::is_same <T, Collider>())
	{
		Debug::PrintError("[JsonToVariable] not implemented for std::weak_ptr<Collider>!");
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr)
{
	ReflectionUtils::FillFileReference<T>(jsonValue, valuePtr);
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr)
{
	const size_t arraySize = jsonValue.size();
	const size_t vectorSize = valuePtr->get().size();

	for (size_t i = 0; i < arraySize; i++)
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
				Debug::PrintError("[JsonToVariable] not implemented for std::vector<std::weak_ptr<Collider>>!");
			}
		}
		if (i >= vectorSize)
		{
			valuePtr->get().push_back(tempVariable);
		}
		else
		{
			valuePtr->get()[i] = tempVariable;
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
ReflectionUtils::JsonToVariable(const json& jsonValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr)
{
	ReflectionUtils::FillVectorFileReference(jsonValue, valuePtr);
}

void ReflectionUtils::JsonToReflectiveData(const json& json, const ReflectiveData& dataList)
{
	if (json.contains("Values"))
	{
		// Go through json Values list
		for (const auto& kv : json["Values"].items())
		{
			// Check if the data list contains the variable name found in the json
			if (dataList.contains(kv.key()))
			{
				const VariableReference& variableRef = dataList.at(kv.key()).variable.value();
				const auto& kvValue = kv.value();
				std::visit([&kvValue](const auto& value)
					{
						JsonToVariable(kvValue, &value);
					}, variableRef);
			}
		}
	}
}

void ReflectionUtils::ReflectiveToReflective(Reflective& fromReflective, Reflective& toReflective)
{
	ReflectiveData fromReflectiveData = fromReflective.GetReflectiveData();
	json jsonData;
	jsonData["Values"] = ReflectiveDataToJson(fromReflectiveData);

	ReflectiveData toReflectiveData = toReflective.GetReflectiveData();
	JsonToReflectiveData(jsonData, toReflectiveData);
	toReflective.OnReflectionUpdated();
}

void ReflectionUtils::JsonToReflective(const json& j, Reflective& reflective)
{
	auto myMap = reflective.GetReflectiveData();
	JsonToReflectiveData(j, myMap);
	reflective.OnReflectionUpdated();
}

#pragma endregion

#pragma region Fill json

// Template for basic types (int, float, strings...)
template<typename T>
std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<T>* valuePtr)
{
	jsonValue[key] = valuePtr->get();
}

void ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<Reflective>* valuePtr)
{
	jsonValue[key]["Values"] = ReflectionUtils::ReflectiveToJson(valuePtr->get());
}

void ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<Reflective>>* valuePtr)
{
	Debug::PrintError("[VariableToJson] not implemented for std::vector<Reflective>!");

	std::vector <Reflective>& getVal = valuePtr->get();
	size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		jsonValue[key]["Values"][vIndex] = ReflectionUtils::ReflectiveToJson(getVal[vIndex]);
		/*if (getVal.at(vIndex).lock())
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
				Debug::PrintError("[VariableToJson] not implemented for std::vector<std::weak_ptr<Collider>>!");
			}
		}*/
	}

}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr)
{
	if (auto lockValue = (valuePtr->get()).lock())
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
			Debug::PrintError("[VariableToJson] not implemented for std::weak_ptr<Collider>!");
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr)
{
	if (valuePtr->get() != nullptr)
		jsonValue[key] = valuePtr->get()->fileId;
}

template<typename T>
std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr)
{
	const std::vector <std::weak_ptr<T>>& getVal = valuePtr->get();
	size_t vectorSize = getVal.size();
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
				Debug::PrintError("[VariableToJson] not implemented for std::vector<std::weak_ptr<Collider>>!");
			}
		}
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
ReflectionUtils::VariableToJson(json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr)
{
	const std::vector <std::shared_ptr<T>>& getVal = valuePtr->get();
	size_t vectorSize = getVal.size();
	for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
	{
		if (getVal.at(vIndex))
			jsonValue[key][vIndex] = getVal.at(vIndex)->fileId;
	}
}

json ReflectionUtils::ReflectiveDataToJson(const ReflectiveData& dataList)
{
	json json;
	for (const auto& kv : dataList)
	{
		const std::string key = kv.first;
		const VariableReference& variableRef = dataList.at(kv.first).variable.value();

		std::visit([&key, &json](const auto& value)
			{
				VariableToJson(json, key, &value);
			}, variableRef);
	}
	return json;
}

json ReflectionUtils::ReflectiveToJson(Reflective& reflective)
{
	const auto dataList = reflective.GetReflectiveData();
	const json jsonData = ReflectiveDataToJson(dataList);
	return jsonData;
}

template <typename T>
void ReflectionUtils::FillFileReference(const uint64_t fileId, const std::reference_wrapper<std::shared_ptr<T>>* variablePtr)
{
	std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId); // Try to find the file reference
	if (file)
	{
		// Load file data
		file->LoadFileReference();
		//Put the file in the variable reference
		variablePtr->get() = std::dynamic_pointer_cast<T>(file);
	}
}

template <typename T>
void ReflectionUtils::FillVectorFileReference(const json& jsonVectorData, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* vectorRefPtr)
{
	const size_t jsonArraySize = jsonVectorData.size();
	const size_t vectorSize = vectorRefPtr->get().size();

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
			vectorRefPtr->get().push_back(std::dynamic_pointer_cast<T>(file));
		}
		else
		{
			vectorRefPtr->get()[i] = std::dynamic_pointer_cast<T>(file);
		}
	}
}


//template <typename T>
//void ReflectionUtils::FillVector(json kvValue, std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr)
//{
//	size_t arraySize = kvValue.size();
//
//	size_t vectorSize = valuePtr->get().size();
//	for (size_t i = 0; i < arraySize; i++)
//	{
//		std::shared_ptr<FileReference> file = nullptr;
//		if (!kvValue.at(i).is_null())
//		{
//			int fileId = kvValue.at(i);
//			file = ProjectManager::GetFileReferenceById(fileId);
//			if (file)
//				file->LoadFileReference();
//		}
//		if (i >= vectorSize)
//		{
//			valuePtr->get().push_back(std::dynamic_pointer_cast<T>(file));
//		}
//		else
//		{
//			valuePtr->get()[i] = std::dynamic_pointer_cast<T>(file);
//		}
//	}
//}

#pragma endregion

#pragma region IO

bool ReflectionUtils::FileToReflectiveData(std::shared_ptr<File> file, const ReflectiveData& dataList)
{
	bool ok = false;

	if (file->Open(FileMode::ReadOnly))
	{
		const std::string jsonString = file->ReadAll();
		file->Close();
		if (!jsonString.empty())
		{
			json myJson;
			myJson = json::parse(jsonString);
			ReflectionUtils::JsonToReflectiveData(myJson, dataList);
			ok = true;
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

bool ReflectionUtils::ReflectiveDataToFile(const ReflectiveData& dataList, std::shared_ptr<File> file)
{
	bool ok = false;
	json myJson;
	myJson["Values"] = ReflectionUtils::ReflectiveDataToJson(dataList);
	FileSystem::fileSystem->Delete(file->GetPath());
	if (file->Open(FileMode::WriteCreateFile))
	{
		file->Write(myJson.dump(0));
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