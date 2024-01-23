#include "gameplay_utility.h"
#include <engine/game_elements/gameplay_manager.h>

#if defined(EDITOR)
#include <editor/editor.h>
#endif
#include <engine/class_registry/class_registry.h>
#include <engine/reflection/reflection_utils.h>

using json = nlohmann::json;

struct ComponentAndId
{
	std::shared_ptr<Component> newComponent = nullptr;
	uint64_t oldId = 0;
};

struct GameObjectAndId
{
	std::shared_ptr<GameObject> newGameObject = nullptr;
	uint64_t oldId = 0;
};

void DuplicateChild(const std::shared_ptr<GameObject> parent, const std::shared_ptr<GameObject> goToDuplicate, std::vector<ComponentAndId>& ComponentsAndIds, std::vector<GameObjectAndId>& GameObjectsAndIds)
{
	// Create new gameobject
	std::string newGameObjectName = goToDuplicate->name;
#if defined(EDITOR)
	if (parent == nullptr)
		newGameObjectName = Editor::GetIncrementedGameObjectName(goToDuplicate->name);
#endif
	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	ReflectionUtils::ReflectiveToReflective(*goToDuplicate.get(), *newGameObject.get());
	newGameObject->name = newGameObjectName;

	// Set parent 
	if (parent != nullptr)
	{
		newGameObject->SetParent(parent);
	}

	// Set local position/rotation/scale
	std::shared_ptr<Transform> newTransform = newGameObject->GetTransform();
	std::shared_ptr<Transform> transformToDuplicate = goToDuplicate->GetTransform();
	newTransform->SetLocalPosition(transformToDuplicate->GetLocalPosition());
	newTransform->SetLocalRotation(transformToDuplicate->GetLocalRotation());
	newTransform->SetLocalScale(transformToDuplicate->GetLocalScale());

	const size_t componentCount = goToDuplicate->components.size();
	for (size_t i = 0; i < componentCount; i++)
	{
		std::shared_ptr<Component> componentToDuplicate = goToDuplicate->components[i];
		std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentToDuplicate->GetComponentName(), newGameObject);
		newComponent->SetIsEnabled(componentToDuplicate->GetIsEnabled());
		ReflectiveData newReflection = newComponent->GetReflectiveData();
		ReflectiveData reflectionToCopy = componentToDuplicate->GetReflectiveData();

		json copiedValues;
		copiedValues["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectionToCopy);

		ReflectionUtils::JsonToReflectiveData(copiedValues, newReflection);
		newComponent->OnReflectionUpdated();

		ComponentAndId newComponentAndId;
		newComponentAndId.newComponent = newComponent;
		newComponentAndId.oldId = componentToDuplicate->GetUniqueId();
		ComponentsAndIds.push_back(newComponentAndId);
	}

	GameObjectAndId gameObjectAndId;
	gameObjectAndId.newGameObject = newGameObject;
	gameObjectAndId.oldId = goToDuplicate->GetUniqueId();

	GameObjectsAndIds.push_back(gameObjectAndId);

	int childCount = goToDuplicate->childCount;
	for (int i = 0; i < childCount; i++)
	{
		DuplicateChild(newGameObject, goToDuplicate->children[i].lock(), ComponentsAndIds, GameObjectsAndIds);
	}
}

std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& goToDuplicate)
{
	if (!goToDuplicate)
		return nullptr;

	std::vector<ComponentAndId> ComponentsAndIds;
	std::vector<GameObjectAndId> GameObjectsAndIds;
	DuplicateChild(nullptr, goToDuplicate, ComponentsAndIds, GameObjectsAndIds);

	// If a component store in a variable a component/gameobject/transform from the duplicated gameobject, replace the reference by the component/gameobject/transform of the new gameobject
	const size_t componentCount = ComponentsAndIds.size();
	const size_t gameObjectCount = GameObjectsAndIds.size();
	for (size_t componentIndex = 0; componentIndex < componentCount; componentIndex++)
	{
		ReflectiveData newReflection = ComponentsAndIds[componentIndex].newComponent->GetReflectiveData();
		for (auto& kv : newReflection)
		{
			VariableReference& variableRef = kv.second.variable.value();
			if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < componentCount; j++)
					{
						if (valuePtr->get().lock()->GetUniqueId() == ComponentsAndIds[j].oldId)
						{
							valuePtr->get() = ComponentsAndIds[j].newComponent;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get().lock()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get() = GameObjectsAndIds[j].newGameObject;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get().lock()->GetGameObject()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get() = GameObjectsAndIds[j].newGameObject->GetTransform();
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
			{
				size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < componentCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetUniqueId() == ComponentsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = ComponentsAndIds[j].newComponent;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
			{
				size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = GameObjectsAndIds[j].newGameObject;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
			{
				size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetGameObject()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = GameObjectsAndIds[j].newGameObject->GetTransform();
						}
					}
				}
			}
		}
	}

	return GameObjectsAndIds[0].newGameObject;
}

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