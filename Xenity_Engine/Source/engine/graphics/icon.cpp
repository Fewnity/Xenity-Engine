#include "icon.h"
#include <engine/asset_management/asset_manager.h>

ReflectiveData Icon::GetReflectiveData()
{
    return ReflectiveData();
}

std::shared_ptr<Icon> Icon::MakeIcon()
{
	std::shared_ptr<Icon> newFileRef = std::make_shared<Icon>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

