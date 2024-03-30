#include "component.h"
#include <engine/game_elements/gameobject.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/physics/physics_manager.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/lighting/lighting.h>

#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>

#pragma region Constructors / Destructor

Component::Component()
{
}

Component::~Component()
{
}

#pragma endregion

void Component::SetGameObject(const std::shared_ptr<GameObject>& newGameObject)
{
	if (!newGameObject)
		return;

	// Check if the component has been just instanciated
	bool firstUse = false;
	if (this->gameObject.expired())
	{
		GameplayManager::componentsListDirty = true;
		firstUse = true;
	}

	this->gameObject = newGameObject;
	this->transform = newGameObject->GetTransform();

	if (firstUse)
	{
		// Move this code in a OnGameObjectSet function in the specific component?
		std::shared_ptr<Component> thisShared = shared_from_this();
		// If the component is a drawble, add to the drawable list
		if (auto result = std::dynamic_pointer_cast<IDrawable>(thisShared))
		{
			Graphics::AddDrawable(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Lod>(thisShared))
		{
			Graphics::AddLod(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Light>(thisShared))
		{
			AssetManager::AddLight(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Camera>(thisShared))
		{
			Graphics::cameras.push_back(result);
		}
		else if (auto result = std::dynamic_pointer_cast<RigidBody>(thisShared))
		{
			PhysicsManager::rigidBodies.push_back(result);
		}
		else if (auto result = std::dynamic_pointer_cast<BoxCollider>(thisShared))
		{
			PhysicsManager::boxColliders.push_back(result);
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
	if (isEnabled)
		OnDisabled();
	else
		OnEnabled();
}
