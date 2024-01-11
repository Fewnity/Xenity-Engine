#include "gameplay_utility.h"
#include <engine/game_elements/gameplay_manager.h>

void DestroyGameObjectAndChild(const std::shared_ptr<GameObject>& gameObject)
{
	GameplayManager::gameObjectsToDestroy.push_back(gameObject);
	gameObject->waitingForDestroy = true;

	// Remove the destroyed gameobject from his parent's children list
	if (auto parent = gameObject->parent.lock())
	{
		const int parentChildCount = parent->GetChildrenCount();
		for (int i = 0; i < parentChildCount; i++)
		{
			if (parent->children[i].lock() == gameObject)
			{
				parent->children.erase(parent->children.begin() + i);
				parent->childCount--;
				break;
			}
		}
	}

	int childCount = gameObject->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		DestroyGameObjectAndChild(gameObject->children[0].lock());
		i--;
		childCount--;
	}
}


void Destroy(const std::weak_ptr<GameObject>& gameObject)
{
	Destroy(gameObject.lock());
}

void Destroy(const std::weak_ptr<Component>& weakComponent)
{
	Destroy(weakComponent.lock());
}

void Destroy(const std::shared_ptr<GameObject>& gameObject)
{
	if (!gameObject->waitingForDestroy)
	{
		DestroyGameObjectAndChild(gameObject);
	}
}

void Destroy(const std::shared_ptr<Component>& component)
{
	// Remove the component from the his parent's components list
	if (component)
		component->GetGameObject()->RemoveComponent(component);
}