#include "gameobject.h"

GameObject::GameObject()
{
	
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
