#include "component_manager.h"

#include <engine/class_registry/class_registry.h>

Event<size_t> ComponentManager::onComponentDeletedEvent;

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

bool BaseComponentList::IsComponentLocalActive(const std::shared_ptr<Component>& component)
{
	return component->GetGameObjectRaw()->IsLocalActive();
}