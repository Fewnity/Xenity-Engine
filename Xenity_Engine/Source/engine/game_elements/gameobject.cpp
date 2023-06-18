#include "gameobject.h"
#include "../engine.h"
#include <iostream>
#include "../debug/debug.h"

#pragma region Constructors / Destructor

#define DEFAULT_GAMEOBJECT_NAME "GameObject"

GameObject::GameObject()
{
	//Engine::AddGameObject(this);
}

/// <summary>
/// 
/// </summary>
/// <param name="name">Do not use "@" alone</param>
GameObject::GameObject(std::string name)
{
	if (name != "")
		this->name = name;
	else
		this->name = DEFAULT_GAMEOBJECT_NAME;
	//Engine::AddGameObject(this);
}

GameObject::~GameObject()
{
	int componentCount = components.size();
	for (int i = 0; i < componentCount; i++)
	{
		/*int componentToDestroyCount = Engine::componentsToDestroy.size();
		for (int i2 = 0; i2 < componentToDestroyCount; i2++)
		{
			if (Engine::componentsToDestroy[i2] == components[i])
			{
				Engine::componentsToDestroy.erase(Engine::componentsToDestroy.begin() + i2);
			}
		}*/
		//delete components[i];
		components[i]->waitingForDestroy = true;
	}
	this->componentCount = 0;
	components.clear();
	//Engine::gameObjectCount--;

	/*for (int i = 0; i < componentCount; i++)
	{
		delete components[i];
	}
	components.clear();*/
}

void GameObject::Setup()
{
	transform = std::make_shared<Transform>(shared_from_this());
}

#pragma endregion

/// <summary>
/// Add a child the the gameobject
/// </summary>
/// <param name="gameObject"></param>
void GameObject::AddChild(std::weak_ptr<GameObject> weakNewChild)
{
	//Check if the child to add is alrady a child of this gameobject
	bool add = true;
	//int childCount = children.size();
	for (int i = 0; i < childCount; i++)
	{
		if (children[i].lock() == weakNewChild.lock())
		{
			add = false;
			break;
		}
	}

	if (add)
	{
		children.push_back(weakNewChild);
		childCount++;
		if (auto newChild = weakNewChild.lock()) 
		{
			//newChild->parent = std::weak_ptr<GameObject>(this);
			newChild->parent = shared_from_this();
			newChild->transform->OnParentChanged();
			newChild->UpdateActive(newChild);
		}
	}
}

void GameObject::SetParent(std::weak_ptr<GameObject> gameObject)
{
	//gameObject->AddChild(this);
}

/// <summary>
/// Add an existing component
/// </summary>
/// <param name="component"></param>
void GameObject::AddExistingComponent(Component* componentToAdd)
{
	if (componentToAdd == nullptr)
		return;

	components.push_back(componentToAdd);
	componentToAdd->SetGameObject(shared_from_this());
	componentCount++;
	componentToAdd->Awake();
}

#pragma region Find GameObjects

/// <summary>
/// Find all gameobjects with a specific name
/// </summary>
/// <param name="name">GameObjects's name</param>
/// <returns></returns>
std::vector<std::weak_ptr<GameObject>> GameObject::FindGameObjectsByName(const std::string name)
{
	std::vector<std::weak_ptr<GameObject>> foundGameObjects;

	if (name == "@")
		return foundGameObjects;

	std::vector<std::weak_ptr<GameObject>> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (auto gameObject = gameObjects[i].lock())
		{
			if (gameObject->name == name)
				foundGameObjects.push_back(gameObject);
		}
	}
	return foundGameObjects;
}

/// <summary>
/// Find a gameobject with a specific name
/// </summary>
/// <param name="name">GameObject's name</param>
/// <returns>GameObject pointer or nullptr if no one is found</returns>
std::weak_ptr<GameObject> GameObject::FindGameObjectByName(const std::string name)
{
	std::vector<std::weak_ptr<GameObject>> gameObjects = Engine::GetGameObjects();

	if (name == "@")
		return std::weak_ptr<GameObject>();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (auto gameObject = gameObjects[i].lock())
		{
			if (gameObject->name == name)
				return gameObject;
		}
	}
	return std::weak_ptr<GameObject>();
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
		//UpdateActive(this);
	}
}

#pragma endregion

/// <summary>
/// Update gameobject active state. Set the local active value depending of gameobject's parents active state
/// </summary>
/// <param name="changed"></param>
void GameObject::UpdateActive(std::weak_ptr<GameObject> weakChanged)
{
	//if (auto changed = weakChanged.lock())
	//{
	//	bool lastLocalActive = localActive;
	//	if (!changed->GetActive() || (!changed->GetLocalActive() && weakChanged != this)) //if the new parent's state is false, set local active to false
	//	{
	//		localActive = false;
	//	}
	//	else if (active)
	//	{
	//		bool newActive = true;
	//		std::weak_ptr<GameObject> gmToCheck = parent;
	//		/*while (gmToCheck != nullptr)
	//		{
	//			if (!gmToCheck->GetActive() || !gmToCheck->GetLocalActive()) //If a parent is disabled, set local active to false
	//			{
	//				newActive = false;
	//				break;
	//			}
	//			if (gmToCheck == changed)
	//			{
	//				break;
	//			}
	//			gmToCheck = gmToCheck->parent;
	//		}*/
	//		localActive = newActive;
	//	}

	//	//If the gameobject has changed his state
	//	if (lastLocalActive != localActive)
	//	{
	//		//Update children
	//		for (int i = 0; i < childCount; i++)
	//		{
	//			children[i]->UpdateActive(changed);
	//		}
	//	}
	//}
}
