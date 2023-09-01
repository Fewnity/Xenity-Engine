#include "reflection_utils.h"
#include "../game_elements/gameobject.h"
#include "../asset_management/project_manager.h"
#include "../file_system/file_reference.h"
#include "../../xenity.h"

using json = nlohmann::json;

void ReflectionUtils::JsonToMap(json json, std::unordered_map<std::string, Variable> theMap)
{
	for (auto& kv : json["Values"].items())
	{
		if (theMap.contains(kv.key()))
		{
			Variable& variableRef = theMap.at(kv.key());
			if (kv.value().is_object())
			{
				if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
				{
					JsonToReflection(kv.value(), valuePtr->get());
				}
			}
			else
			{
				if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
				{
					auto comp = FindComponentById(kv.value());
					valuePtr->get() = comp;
				}
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					valuePtr->get() = go;
				}
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					valuePtr->get() = go->GetTransform();
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
				{
					int fileId = kv.value();
					std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = std::dynamic_pointer_cast<Texture>(file);
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
				{
					int fileId = kv.value();
					std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = std::dynamic_pointer_cast<MeshData>(file);
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
				{
					int fileId = kv.value();
					std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = std::dynamic_pointer_cast<Scene>(file);
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
				{
					int fileId = kv.value();
					std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = std::dynamic_pointer_cast<AudioClip>(file);
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
				{
					int arraySize = kv.value().size();

					int vectorSize = valuePtr->get().size();
					for (int i = 0; i < arraySize; i++)
					{
						std::shared_ptr<FileReference> file = nullptr;
						if (!kv.value().at(i).is_null())
						{
							int fileId = kv.value().at(i);
							file = ProjectManager::GetFileReferenceById(fileId);
							if(file)
								file->LoadFileReference();
						}
						if (i < vectorSize - 1)
						{
							valuePtr->get()[i] = std::dynamic_pointer_cast<Texture>(file);
						}
						else
						{
							valuePtr->get().push_back(std::dynamic_pointer_cast<Texture>(file));
						}
					}
				}
			}
		}
	}
}

void ReflectionUtils::JsonToReflection(json j, Reflection& reflection)
{
	auto myMap = reflection.GetReflection();
	JsonToMap(j, myMap);
}

json ReflectionUtils::MapToJson(std::unordered_map<std::string, Variable> theMap)
{
	json json;
	for (const auto& kv : theMap)
	{
		Variable& variableRef = theMap.at(kv.first);
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
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
		{
			int s = valuePtr->get().size();
			for (int i = 0; i < s; i++)
			{
				if (valuePtr->get().at(i))
					json[kv.first][i] = valuePtr->get().at(i)->fileId;
			}
		}
	}
	return json;
}

json ReflectionUtils::ReflectionToJson(Reflection& reflection)
{
	json j2;
	auto t = reflection.GetReflection();
	j2 = MapToJson(t);
	return j2;
}