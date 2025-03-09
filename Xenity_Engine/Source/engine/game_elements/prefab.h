#pragma once

#include <json.hpp>
#include <set>
#include <engine/api.h>
#include <engine/file_system/file_reference.h>

class Prefab : public FileReference
{
public:
	Prefab();
	~Prefab();

	static std::shared_ptr<Prefab> MakePrefab();

	ReflectiveData GetReflectiveData();
	void SetData(GameObject& gameObject);
	const nlohmann::json& GetData() const;
	void LoadFileReference(const LoadOptions& loadOptions) override;
private:
	nlohmann::json data;
	void SaveGameObject(GameObject& gameObject, std::set<uint64_t>& usedFilesIds);

	static constexpr int s_version = 1;
};

