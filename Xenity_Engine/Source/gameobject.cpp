#include "gameobject.h"
#include "engine.h"

GameObject::GameObject()
{
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

void GameObject::AddChild(GameObject* gameObject)
{
	bool add = true;
	int childCount = childs.size();
	for (int i = 0; i < childCount; i++)
	{
		if (childs[i] == gameObject)
		{
			add = false;
			break;
		}
	}

	if (add)
		childs.push_back(gameObject);
}

void GameObject::AddComponent(Component* component)
{
	bool add = true;
	int componentCount = components.size();
	for (int i = 0; i < componentCount; i++)
	{
		if (components[i] == component)
		{
			add = false;
			break;
		}
	}

	if (add)
		components.push_back(component);
}