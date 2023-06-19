#include "component.h"
#include "engine.h"
#include <iostream>

#include "graphics/iDrawable.h"
#include "graphics/graphics.h"
#include "game_elements/gameobject.h"
#include "asset_managent/asset_manager.h"

#pragma region Constructors / Destructor

Component::Component()
{
}

Component::~Component()
{
}

#pragma endregion

void Component::SetGameObject(std::weak_ptr<GameObject>go)
{
	if (go.expired())
		return;

	// Check if the component has been just instanciated
	bool firstUse = false;
	if (gameObject.expired())
	{
		Engine::componentsListDirty = true;
		firstUse = true;
	}

	this->gameObject = go;
	this->transform = go.lock()->GetTransform();

	if (firstUse)
	{
		// If the component is a drawble, add to the drawable list
		if (auto result = std::dynamic_pointer_cast<IDrawable>(shared_from_this()))
		{
			Graphics::AddDrawable(result);
			AssetManager::AddDrawable(result);
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
