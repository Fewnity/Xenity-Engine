#include "prefab.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/scene_management/scene_manager.h>

using ordered_json = nlohmann::ordered_json;

Prefab::Prefab()
{
}

Prefab::~Prefab()
{
}

std::shared_ptr<Prefab> Prefab::MakePrefab()
{
	std::shared_ptr<Prefab> newFileRef = std::make_shared<Prefab>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

ReflectiveData Prefab::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, data, "data", true);
	return reflectedVariables;
}

const nlohmann::ordered_json& Prefab::GetData() const
{
	return data;
}

void Prefab::LoadFileReference(const LoadOptions& loadOptions)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		const std::string jsonString = ReadString();
		if (!jsonString.empty())
		{
			ordered_json j;
			try
			{
				j = ordered_json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadFileReference] Failed to load the material file", true);
				m_fileStatus = FileStatus::FileStatus_Failed;
				return;
			}
			ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());

			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			Debug::PrintError("[SkyBox::LoadFileReference] Fail to load the skybox file: " + m_file->GetPath(), true);
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
	}
}

#if defined(EDITOR)
void Prefab::SetData(GameObject& gameObject)
{
	std::set<uint64_t> usedFilesIds;

	SaveGameObject(gameObject, usedFilesIds);

	ordered_json jsonData;
	jsonData["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());
	jsonData["Version"] = s_version;

	const bool saveResult = ReflectionUtils::JsonToFile(jsonData, m_file);
	if (!saveResult)
	{
		Debug::PrintError("[Prefab::OnReflectionUpdated] Fail to save the Prefab file: " + m_file->GetPath(), true);
	}
}

void Prefab::SaveGameObject(GameObject& gameObject, std::set<uint64_t>& usedFilesIds)
{
	const std::string gameObjectId = std::to_string(gameObject.GetUniqueId());
	data[gameObjectId] = SceneManager::GameObjectToJson(gameObject, usedFilesIds);

	// Save children
	const size_t childrenCount = gameObject.GetChildrenCount();
	for (size_t i = 0; i < childrenCount; i++)
	{
		SaveGameObject(*gameObject.GetChild(static_cast<int>(i)).lock(), usedFilesIds);
	}
}
#endif
