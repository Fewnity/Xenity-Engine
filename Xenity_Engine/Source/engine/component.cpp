#include "component.h"
#include "engine.h"

#pragma region Constructors / Destructor

Component::Component()
{
	Engine::componentsListDirty = true;
}

Component::~Component()
{
}

#pragma endregion

bool Component::GetIsEnabled()
{
	return isEnabled;
}

void Component::SetIsEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}
