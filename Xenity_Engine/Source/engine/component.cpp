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
#include "asset_managent/asset_manager.h"

#pragma region Constructors / Destructor

Component::Component()
{
}

Component::~Component()
{
}

#pragma endregion

void Component::SetGameObject(std::weak_ptr<GameObject> newGameObject)
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
		// If the component is a drawble, add to the drawable list
		if (auto result = std::dynamic_pointer_cast<IDrawable>(shared_from_this()))
		{
			Graphics::AddDrawable(result);
			AssetManager::AddDrawable(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Light>(shared_from_this()))
		{
			AssetManager::AddLight(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Camera>(shared_from_this()))
		{
			if (Graphics::usedCamera.expired())
				Graphics::usedCamera = std::dynamic_pointer_cast<Camera>(shared_from_this());
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
