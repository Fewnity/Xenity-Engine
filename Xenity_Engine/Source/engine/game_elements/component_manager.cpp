#include "component_manager.h"

#include <engine/class_registry/class_registry.h>

bool ComponentManager::GetCompnentDisabledLoop(size_t typeId)
{
	const ClassRegistry::ClassInfo* classInfo = ClassRegistry::GetClassInfoById(typeId);
	if (classInfo != nullptr)
	{
		return classInfo->disableUpdateLoop;
	}

	XASSERT(false, "[ComponentManager::GetCompnentDisabledLoop] ClassInfo not found");
	return false;
}
