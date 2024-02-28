#include "missing_script.h"
#include "asset_management/asset_manager.h"

using json = nlohmann::json;

MissingScript::MissingScript()
{
	componentName = "MissingScript";
	AssetManager::AddReflection(this);
}

MissingScript::~MissingScript()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData MissingScript::GetReflectiveData()
{
	return ReflectiveData();
}
