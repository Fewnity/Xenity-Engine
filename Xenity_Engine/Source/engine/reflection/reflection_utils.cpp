#include "reflection_utils.h"
#include "../game_elements/gameobject.h"
#include "../asset_management/project_manager.h"
#include "../file_system/file_reference.h"
#include "../../xenity.h"

using json = nlohmann::json;

void ReflectionUtils::JsonToMap(std::unordered_map<std::string, Variable> t, json json)
{
	for (auto& kv : json["Values"].items())
	{
		if (t.contains(kv.key()))
		{
			Variable& variableRef = t.at(kv.key());
			if (kv.value().is_object())
			{
				//std::cout << kv.key() << " is an object!" << "\n";
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
				else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go;
				}
				else if (auto valuePtr = std::get_if<std::weak_ptr<Transform>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go->GetTransform();
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
				{
					int fileId = kv.value();
					FileReference* file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = (Texture*)file;
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
				{
					int fileId = kv.value();
					FileReference* file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = (MeshData*)file;
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<Scene*>>(&variableRef))
				{
					int fileId = kv.value();
					FileReference* file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = (Scene*)file;
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<Texture*>>>(&variableRef))
				{
					int arraySize = kv.value().size();

					int vectorSize = valuePtr->get().size();
					for (int i = 0; i < arraySize; i++)
					{
						FileReference* file = nullptr;
						if (!kv.value().at(i).is_null())
						{
							int fileId = kv.value().at(i);
							file = ProjectManager::GetFileReferenceById(fileId);
							if(file)
								file->LoadFileReference();
						}
						if (i < vectorSize - 1)
						{
							valuePtr->get()[i] = (Texture*)file;
						}
						else
						{
							valuePtr->get().push_back((Texture*)file);
						}
					}
				}
				/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
				{
					std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);

					for (int compI = 0; compI < allCreatedComponentsCount; compI++)
					{
						if (allCreatedComponents[compI]->GetUniqueId() == kv.value())
						{
							*weakC = allCreatedComponents[compI];
							break;
						}
					}
				}*/
			}
		}
		// std::cout << kv.key() << " : " << kv.value() << "\n";
	}
}

void ReflectionUtils::JsonToReflection(json j, Reflection& component)
{
	auto myMap = component.GetReflection();
	JsonToMap(myMap, j);
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
		else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				json[kv.first] = lockValue->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::weak_ptr<Transform> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				json[kv.first] = lockValue->GetGameObject()->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			json[kv.first]["Values"] = ReflectiveToJson(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
				json[kv.first]["Values"] = ReflectiveToJson(*lockValue.get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<AudioClip*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Scene*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				json[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<Texture*>>>(&variableRef))
		{
			int s = valuePtr->get().size();
			for (int i = 0; i < s; i++)
			{
				if (valuePtr->get().at(i))
					json[kv.first][i] = valuePtr->get().at(i)->fileId;
			}
		}
		/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
		{
			std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);
			if (auto lockValue = weakC->lock())
			{
				j2[kv.first] = lockValue->GetUniqueId();
			}
		}*/
	}
	return json;
}

json ReflectionUtils::ReflectiveToJson(Reflection& relection)
{
	json j2;
	auto t = relection.GetReflection();
	j2 = MapToJson(t);
	return j2;
}