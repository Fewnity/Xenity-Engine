#include "gameobject.h"
#include "engine.h"
#include <iostream>

GameObject::GameObject()
{
	Engine::AddGameObject(this);
}

GameObject::GameObject(std::string name)
{
	this->name = name;
	Engine::AddGameObject(this);
}

GameObject::~GameObject()
{
	int componentsCount = components.size();
	for (int i = 0; i < componentsCount; i++)
	{
		delete components[i];
	}
	components.clear();
}


/// <summary>
/// Add a child the the gameobject
/// </summary>
/// <param name="gameObject"></param>
void GameObject::AddChild(GameObject* newChild)
{
	//Check if the child to add is alrady a child of this gameobject
	bool add = true;
	int childCount = children.size();
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
		newChild->parent = this;
		newChild->transform.OnParentChanged();
		newChild->transform.UpdateLocalScale();
	}
}

/// <summary>
/// Add an existing component
/// </summary>
/// <param name="component"></param>
void GameObject::AddExistingComponent(Component* componentToAdd)
{
	//Check if the component to add is alrady a component of this gameobject
	bool add = true;
	int componentCount = components.size();
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
	}
}

/// <summary>
/// Find all gameobjects with a specific name
/// </summary>
/// <param name="name">GameObjects's name</param>
/// <returns></returns>
std::vector<GameObject*> GameObject::FindGameObjectsByName(std::string name)
{
	std::vector<GameObject*> foundGameObjects;

	Engine::GetGameObjects().clear();
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = gameObjects.size();

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
GameObject* GameObject::FindGameObjectByName(std::string name)
{
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();
	int gameObjectCount = gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			return gameObjects[i];
	}
	return nullptr;
}

bool GameObject::GetActive()
{
	return active;
}

bool GameObject::GetLocalActive()
{
	return localActive;
}

void GameObject::SetActive(bool active)
{
	if (active != this->active)
	{
		this->active = active;
		UpdateActive(this);
	}
}

/// <summary>
/// Update gameobject active state. Set the local active value depending of gameobject's parents active state
/// </summary>
/// <param name="changed"></param>
void GameObject::UpdateActive(GameObject* changed)
{
	bool lastLocalActive = localActive;
	if (!changed->GetActive()) //if the new parent's state is false, set local active to false
	{
		localActive = false;
	}
	else if(active)
	{
		bool newActive = true;
		GameObject* gmToCheck = parent;
		while (gmToCheck != nullptr)
		{
			if (!gmToCheck->GetActive()) //If a parent is disabled, set local active to false
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
		int childCount = children.size();
		for (int i = 0; i < childCount; i++)
		{
			children[i]->UpdateActive(changed);
		}
	}
}
