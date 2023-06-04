#include "component.h"
#include "engine.h"
#include <iostream>

#include "graphics/iDrawable.h"
#include "graphics/graphics.h"
#include "game_elements/gameobject.h"

#pragma region Constructors / Destructor

Component::Component()
{
}

Component::~Component()
{
}

#pragma endregion

void Component::SetGameObject(GameObject *go)
{
	if (go == nullptr)
		return;

	// Check if the component has been just instanciated
	bool firstUse = false;
	if (gameObject == nullptr)
	{
		Engine::componentsListDirty = true;
		firstUse = true;
	}

	this->gameObject = go;
	this->transform = go->GetTransform();

	if (firstUse)
	{
		// If the component is a drawble, add to the drawable list
		if (IDrawable *result = dynamic_cast<IDrawable *>(this))
		{
			Graphics::AddDrawable(result);
		}
	}
}

bool Component::GetIsEnabled()
{
	return isEnabled;
}

void Component::SetIsEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}
