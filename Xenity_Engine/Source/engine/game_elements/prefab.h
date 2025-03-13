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

	ReflectiveData GetReflectiveData() override;
#if defined(EDITOR)
	void SetData(GameObject& gameObject);
#endif
	const nlohmann::json& GetData() const;
	void LoadFileReference(const LoadOptions& loadOptions) override;
private:
#if defined(EDITOR)
	void SaveGameObject(GameObject& gameObject, std::set<uint64_t>& usedFilesIds);
#endif
	nlohmann::json data;
	static constexpr int s_version = 1;
};

