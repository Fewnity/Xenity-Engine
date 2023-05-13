#include "gameobject.h"
#include "../engine.h"
#include <iostream>
#include "../debug/debug.h"

#pragma region Constructors / Destructor

#define DEFAULT_GAMEOBJECT_NAME "GameObject"

GameObject::GameObject()
{
	Engine::AddGameObject(this);
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
	Engine::AddGameObject(this);
}

GameObject::~GameObject()
{
	for (int i = 0; i < componentCount; i++)
	{
		delete components[i];
	}
	components.clear();
}

#pragma endregion

/// <summary>
/// Add a child the the gameobject
/// </summary>
/// <param name="gameObject"></param>
void GameObject::AddChild(GameObject* newChild)
{
	//Check if the child to add is alrady a child of this gameobject
	bool add = true;
	//int childCount = children.size();
	for (int i = 0; i < childCount; i++)
	{
		if (children[i] == newChild)
		{
			add = false;
			break;
		}
	}

	if (add)
	{
		children.push_back(newChild);
		childCount++;
		newChild->parent = this;
		newChild->transform.OnParentChanged();
		newChild->UpdateActive(newChild);
	}
}

void GameObject::SetParent(GameObject* gameObject)
{
	gameObject->AddChild(this);
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
	componentToAdd->gameObject = this;
	componentCount++;

	//Check if the component to add is alrady a component of this gameobject
	/*bool add = true;
	for (int i = 0; i < componentCount; i++)
	{
		if (components[i] == componentToAdd)
		{
			add = false;
			break;
		}
	}

	if (add)
	{
		components.push_back(componentToAdd);
		componentToAdd->gameObject = this;
		componentCount++;
	}*/
}

#pragma region Find GameObjects

/// <summary>
/// Find all gameobjects with a specific name
/// </summary>
/// <param name="name">GameObjects's name</param>
/// <returns></returns>
std::vector<GameObject*> GameObject::FindGameObjectsByName(const std::string name)
{
	std::vector<GameObject*> foundGameObjects;

	if(name == "@")
		return foundGameObjects;

	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			foundGameObjects.push_back(gameObjects[i]);
	}
	return foundGameObjects;
}

/// <summary>
/// Find a gameobject with a specific name
/// </summary>
/// <param name="name">GameObject's name</param>
/// <returns>GameObject pointer or nullptr if no one is found</returns>
GameObject* GameObject::FindGameObjectByName(const std::string name)
{
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();

	if (name == "@")
		return nullptr;

	int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			return gameObjects[i];
	}
	return nullptr;
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
		UpdateActive(this);
	}
}

#pragma endregion

/// <summary>
/// Update gameobject active state. Set the local active value depending of gameobject's parents active state
/// </summary>
/// <param name="changed"></param>
void GameObject::UpdateActive(GameObject* changed)
{
	bool lastLocalActive = localActive;
	if (!changed->GetActive() || (!changed->GetLocalActive() && changed != this)) //if the new parent's state is false, set local active to false
	{
		localActive = false;
	}
	else if (active)
	{
		bool newActive = true;
		GameObject* gmToCheck = parent;
		while (gmToCheck != nullptr)
		{
			if (!gmToCheck->GetActive() || !gmToCheck->GetLocalActive()) //If a parent is disabled, set local active to false
			{
				newActive = false;
				break;
			}
			if (gmToCheck == changed)
			{
				break;
			}
			gmToCheck = gmToCheck->parent;
		}
		localActive = newActive;
	}

	//If the gameobject has changed his state
	if (lastLocalActive != localActive)
	{
		//Update children
		for (int i = 0; i < childCount; i++)
		{
			children[i]->UpdateActive(changed);
		}
	}
}
