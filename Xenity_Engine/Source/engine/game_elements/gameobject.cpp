#include "gameobject.h"
#include "../engine.h"
#include <iostream>
#include "../debug/debug.h"
#include "../graphics/graphics.h"
#include "../asset_management/asset_manager.h"
#include "../graphics/iDrawable.h"
#include "../audio/audio_source.h"
#include "../audio/audio_manager.h"
#include "../graphics/camera.h"
#include "../lighting/lighting.h"

#pragma region Constructors / Destructor

#define DEFAULT_GAMEOBJECT_NAME "GameObject"

std::shared_ptr<GameObject> CreateGameObject()
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>();
	Engine::AddGameObject(newGameObject);
	newGameObject->Setup();
	return newGameObject;
}

std::shared_ptr<GameObject> CreateGameObject(std::string name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	Engine::AddGameObject(newGameObject);
	newGameObject->Setup();
	return newGameObject;
}

std::shared_ptr<GameObject> CreateGameObjectEditor(std::string name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	Engine::AddGameObjectEditor(newGameObject);
	newGameObject->Setup();
	return newGameObject;
}

std::shared_ptr<Component> FindComponentById(const uint64_t id)
{
	for (int i = 0; i < Engine::gameObjectCount; i++)
	{
		std::shared_ptr<GameObject> gameobject = Engine::gameObjects[i];
		int componentCount = gameobject->GetComponentCount();
		for (int compI = 0; compI < componentCount; compI++)
		{
			if (gameobject->components[compI]->GetUniqueId() == id)
			{
				return gameobject->components[compI];
			}
		}
	}
	return std::shared_ptr<Component>();
}

GameObject::GameObject()
{
	this->name = DEFAULT_GAMEOBJECT_NAME;
	//SetReflection();
}

GameObject::GameObject(std::string name)
{
	if (name != "")
		this->name = name;
	else
		this->name = DEFAULT_GAMEOBJECT_NAME;
	//SetReflection();
}

/*void GameObject::SetReflection()
{
	reflectedVariables["name"] = &name;
	reflectedVariables["active"] = &active;
}*/

std::unordered_map<std::string, ReflectionEntry> GameObject::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, name, "name", true);
	Reflection::AddReflectionVariable(reflectedVariables, active, "active", true);
	return reflectedVariables;
}

GameObject::~GameObject()
{
	for (int i = 0; i < componentCount; i++)
	{
		InternalDestroyComponent(components[i]);
	}
	components.clear();
}

void GameObject::Setup()
{
	transform = std::make_shared<Transform>(shared_from_this());
}

#pragma endregion

void GameObject::InternalDestroyComponent(std::weak_ptr <Component> weakComponent)
{
	if (auto component = weakComponent.lock())
	{
		//------------------------------------------------------------------------ Include the component to compile
		if (auto drawable = std::dynamic_pointer_cast<IDrawable>(component))
		{
			Graphics::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(component));
			AssetManager::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(component));
		}
		else if (auto light = std::dynamic_pointer_cast<Light>(component))
		{
			AssetManager::RemoveLight(light);
		}
		else if (auto audioSource = std::dynamic_pointer_cast<AudioSource>(component))
		{
			AudioManager::RemoveAudioSource(audioSource);
		}
		else if (auto camera = std::dynamic_pointer_cast<Camera>(component))
		{
			int cameraCount = Graphics::cameras.size();
			for (int i = 0; i < cameraCount; i++)
			{
				auto cam = Graphics::cameras[i].lock();
				if (cam && cam == camera)
				{
					Graphics::cameras.erase(Graphics::cameras.begin() + i);
					break;
				}
			}
		}
	}
}

void GameObject::RemoveComponent(std::weak_ptr<Component> weakComponent)
{
	if (auto component = weakComponent.lock())
	{
		if (!component->waitingForDestroy)
		{
			component->waitingForDestroy = true;
			Engine::componentsToDestroy.push_back(component);
			for (int i2 = 0; i2 < componentCount; i2++)
			{
				if (components[i2] == component)
				{
					components.erase(components.begin() + i2);
					componentCount--;
					break;
				}
			}
		}
	}
}

void GameObject::AddChild(std::weak_ptr<GameObject> weakNewChild)
{
	if (auto newChild = weakNewChild.lock())
	{
		// Check if the child to add is alrady a child of this gameobject
		bool add = true;
		for (int i = 0; i < childCount; i++)
		{
			if (children[i].lock() == newChild)
			{
				add = false;
				break;
			}
		}

		if (add)
		{
			children.push_back(weakNewChild);
			childCount++;
			newChild->parent = shared_from_this();
			newChild->transform->OnParentChanged();
			newChild->UpdateActive(newChild);
		}
	}
}

void GameObject::SetParent(std::weak_ptr<GameObject> gameObject)
{
	gameObject.lock()->AddChild(shared_from_this());
}

void GameObject::AddExistingComponent(std::shared_ptr<Component> componentToAdd)
{
	if (!componentToAdd.get())
		return;

	components.push_back(componentToAdd);
	componentToAdd->SetGameObject(shared_from_this());
	componentCount++;
	if (Engine::GetGameState() == Playing && GetLocalActive()) 
	{
		componentToAdd->Awake();
		componentToAdd->isAwakeCalled = true;
	}
}

#pragma region Find GameObjects

std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string name)
{
	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	if (name == "@")
		return foundGameObjects;

	std::vector<std::shared_ptr<GameObject>> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			foundGameObjects.push_back(gameObjects[i]);
	}
	return foundGameObjects;
}

std::shared_ptr<GameObject> FindGameObjectByName(const std::string name)
{
	std::vector<std::shared_ptr<GameObject>> gameObjects = Engine::GetGameObjects();

	if (name == "@")
		return std::shared_ptr<GameObject>();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			return gameObjects[i];
	}
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id)
{
	std::vector<std::shared_ptr<GameObject>> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetUniqueId() == id)
			return gameObjects[i];
	}
	return std::shared_ptr<GameObject>();
}

#pragma endregion

#pragma region Accessors

bool GameObject::GetActive() const
{
	return active;
}

bool GameObject::GetLocalActive() const
{
	return localActive;
}

void GameObject::SetActive(const bool active)
{
	if (active != this->active)
	{
		this->active = active;
		UpdateActive(shared_from_this());
	}
}

#pragma endregion

void GameObject::UpdateActive(std::weak_ptr<GameObject> weakChanged)
{
	if (auto changed = weakChanged.lock())
	{
		bool lastLocalActive = localActive;
		if (!changed->GetActive() || (!changed->GetLocalActive() && changed != shared_from_this())) // if the new parent's state is false, set local active to false
		{
			localActive = false;
		}
		else if (active)
		{
			bool newActive = true;
			std::weak_ptr<GameObject> gmToCheck = parent;
			while (!gmToCheck.expired())
			{
				auto gm = gmToCheck.lock();

				if (!gm->GetActive() || !gm->GetLocalActive()) // If a parent is disabled, set local active to false
				{
					newActive = false;
					break;
				}
				if (gm == changed)
				{
					break;
				}
				gmToCheck = gm->parent;
			}
			localActive = newActive;
		}

		// If the gameobject has changed his state
		if (lastLocalActive != localActive)
		{
			// Update children
			for (int i = 0; i < childCount; i++)
			{
				children[i].lock()->UpdateActive(changed);
			}
		}
	}
}

void GameObject::OnReflectionUpdated()
{
	UpdateActive(shared_from_this());
}
