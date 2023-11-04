#include "reflection_utils.h"
#include "../game_elements/gameobject.h"
#include "../asset_management/project_manager.h"
#include "../file_system/file_reference.h"
#include "../../xenity.h"

using json = nlohmann::json;

template <typename T>
void ReflectionUtils::FillFileReference(uint64_t fileId, std::reference_wrapper<std::shared_ptr<T>>* valuePtr)
{
	std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId);
	if (file)
	{
		file->LoadFileReference();
		valuePtr->get() = std::dynamic_pointer_cast<T>(file);
	}
}

template <typename T>
void ReflectionUtils::FillVectorFileReference(json kvValue, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr)
{
	size_t arraySize = kvValue.size();

	size_t vectorSize = valuePtr->get().size();
	for (size_t i = 0; i < arraySize; i++)
	{
		std::shared_ptr<FileReference> file = nullptr;
		if (!kvValue.at(i).is_null())
		{
			uint64_t fileId = kvValue.at(i);
			file = ProjectManager::GetFileReferenceById(fileId);
			if (file)
				file->LoadFileReference();
		}
		if (i >= vectorSize)
		{
			valuePtr->get().push_back(std::dynamic_pointer_cast<T>(file));
		}
		else
		{
			valuePtr->get()[i] = std::dynamic_pointer_cast<T>(file);
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

void ReflectionUtils::JsonToMap(const json& json, std::unordered_map<std::string, ReflectionEntry> theMap)
{
	if (json.contains("Values"))
	{
		for (auto& kv : json["Values"].items())
		{
			if (theMap.contains(kv.key()))
			{
				Variable& variableRef = theMap.at(kv.key()).variable.value();
				auto& kvValue = kv.value();
				if (kvValue.is_object())
				{
					if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
					{
						JsonToReflection(kvValue, valuePtr->get());
					}
				}
				else
				{
					if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
						valuePtr->get() = kvValue;
					else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
						valuePtr->get() = kvValue;
					else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))
						valuePtr->get() = kvValue;
					else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
						valuePtr->get() = kvValue;
					else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
						valuePtr->get() = kvValue;
					else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
					{
						auto comp = FindComponentById(kvValue);
						valuePtr->get() = comp;
					}
					else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef))
					{
						auto go = FindGameObjectById(kvValue);
						valuePtr->get() = go;
					}
					else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef))
					{
						auto go = FindGameObjectById(kvValue);
						valuePtr->get() = go->GetTransform();
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
					{
						FillFileReference<Texture>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
					{
						FillFileReference<MeshData>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
					{
						FillFileReference<Scene>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
					{
						FillFileReference<AudioClip>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<SkyBox>>>(&variableRef))
					{
						FillFileReference<SkyBox>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Font>>>(&variableRef))
					{
						FillFileReference<Font>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Shader>>>(&variableRef))
					{
						FillFileReference<Shader>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Material>>>(&variableRef))
					{
						FillFileReference<Material>(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Font>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Material>>>>(&variableRef))
					{
						FillVectorFileReference(kvValue, valuePtr);
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
					{
						size_t arraySize = kvValue.size();

						size_t vectorSize = valuePtr->get().size();
						for (size_t i = 0; i < arraySize; i++)
						{
							std::shared_ptr<Component> comp = nullptr;
							if (!kvValue.at(i).is_null())
							{
								uint64_t id = kvValue.at(i);
								comp = FindComponentById(id);
							}
							if (i >= vectorSize)
							{
								valuePtr->get().push_back(comp);
							}
							else
							{
								valuePtr->get()[i] = comp;
							}
						}
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
					{
						size_t arraySize = kvValue.size();

						size_t vectorSize = valuePtr->get().size();
						for (size_t i = 0; i < arraySize; i++)
						{
							std::shared_ptr<GameObject> go = nullptr;
							if (!kvValue.at(i).is_null())
							{
								uint64_t id = kvValue.at(i);
								go = FindGameObjectById(id);
							}
							if (i >= vectorSize)
							{
								valuePtr->get().push_back(go);
							}
							else
							{
								valuePtr->get()[i] = go;
							}
						}
					}
					else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
					{
						size_t arraySize = kvValue.size();

						size_t vectorSize = valuePtr->get().size();
						for (size_t i = 0; i < arraySize; i++)
						{
							std::shared_ptr<Transform> trans = nullptr;
							if (!kvValue.at(i).is_null())
							{
								uint64_t id = kvValue.at(i);
								std::shared_ptr <GameObject> go = FindGameObjectById(id);
								if (go)
								{
									trans = go->GetTransform();
								}
							}
							if (i >= vectorSize)
							{
								valuePtr->get().push_back(trans);
							}
							else
							{
								valuePtr->get()[i] = trans;
							}
						}
					}
				}
			}
		}
	}
}

void ReflectionUtils::JsonToReflection(const json& j, Reflection& reflection)
{
	auto myMap = reflection.GetReflection();
	JsonToMap(j, myMap);
	reflection.OnReflectionUpdated();
}

json ReflectionUtils::MapToJson(std::unordered_map<std::string, ReflectionEntry> theMap)
{
	json json;
	for (const auto& kv : theMap)
	{
		Variable& variableRef = theMap.at(kv.first).variable.value();
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
			json[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
			json[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<double>>(&variableRef))
			json[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
			json[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
			json[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject> >>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
				json[kv.first] = lockValue->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform> >>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
				json[kv.first] = lockValue->GetGameObject()->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			json[kv.first]["Values"] = ReflectionToJson(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
				json[kv.first] = lockValue->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<SkyBox>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Font>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Shader>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Material>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = valuePtr->get()->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<Texture>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<MeshData>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<AudioClip>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<Scene>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<SkyBox>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Font>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<Font>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<Shader>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Material>>>>(&variableRef))
		{
			const std::vector <std::shared_ptr<Material>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex))
					json[kv.first][vIndex] = getVal.at(vIndex)->fileId;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
		{
			const std::vector <std::weak_ptr<Component>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex).lock())
					json[kv.first][vIndex] = getVal.at(vIndex).lock()->GetUniqueId();
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
		{
			const std::vector <std::weak_ptr<GameObject>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex).lock())
					json[kv.first][vIndex] = getVal.at(vIndex).lock()->GetUniqueId();
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
		{
			const std::vector <std::weak_ptr<Transform>>& getVal = valuePtr->get();
			size_t vectorSize = getVal.size();
			for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
			{
				if (getVal.at(vIndex).lock())
					json[kv.first][vIndex] = getVal.at(vIndex).lock()->GetGameObject()->GetUniqueId();
			}
		}
	}
	return json;
}

bool ReflectionUtils::FileToMap(std::shared_ptr<File> file, std::unordered_map<std::string, ReflectionEntry> theMap)
{
	bool ok;

	json myJson;
	if (file->Open(true))
	{
		std::string jsonString = file->ReadAll();
		file->Close();
		if (!jsonString.empty())
		{
			myJson = json::parse(jsonString);
			ReflectionUtils::JsonToMap(myJson, theMap);
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

bool ReflectionUtils::MapToFile(std::unordered_map<std::string, ReflectionEntry> theMap, std::shared_ptr<File> file)
{
	bool ok;
	json myJson;
	myJson["Values"] = ReflectionUtils::MapToJson(theMap);
	FileSystem::fileSystem->DeleteFile(file->GetPath());
	if (file->Open(true))
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

json ReflectionUtils::ReflectionToJson(Reflection& reflection)
{
	json j2;
	auto t = reflection.GetReflection();
	j2 = MapToJson(t);
	return j2;
}