#include "component.h"
#include "engine.h"
#include <iostream>

#include "graphics/iDrawable.h"
#include "graphics/camera.h"
#include "lighting/lighting.h"
#include "audio/audio_source.h"
#include "audio/audio_manager.h"
#include "graphics/graphics.h"
#include "game_elements/gameobject.h"
#include "asset_management/asset_manager.h"

#pragma region Constructors / Destructor

Component::Component()
{
}

Component::~Component()
{
}

#pragma endregion

void Component::SetGameObject(const std::weak_ptr<GameObject>& newGameObject)
{
	if (newGameObject.expired())
		return;

	// Check if the component has been just instanciated
	bool firstUse = false;
	if (this->gameObject.expired())
	{
		Engine::componentsListDirty = true;
		firstUse = true;
	}

	this->gameObject = newGameObject;
	this->transform = newGameObject.lock()->GetTransform();

	if (firstUse)
	{
		std::shared_ptr<Component> thisShared = shared_from_this();
		// If the component is a drawble, add to the drawable list
		if (auto result = std::dynamic_pointer_cast<IDrawable>(thisShared))
		{
			Graphics::AddDrawable(result);
			AssetManager::AddDrawable(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Light>(thisShared))
		{
			AssetManager::AddLight(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Camera>(thisShared))
		{
			Graphics::cameras.push_back(result);
		}
	}
}

bool Component::GetIsEnabled() const
{
	return isEnabled;
}

void Component::SetIsEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}
