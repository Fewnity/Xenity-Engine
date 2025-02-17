#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <engine/api.h>

#include <engine/component.h>

class BaseComponentList
{
public:
	/**
	* @brief Constructor
	*/
	BaseComponentList(size_t maxComponentCount, bool disabledLoop)
	{
		m_maxComponentCount = maxComponentCount;
		m_disabledLoop = disabledLoop;
	}

	/**
	* @brief Create a new component of the child template type (Will create a new list if no slot available)
	*/
	virtual std::shared_ptr<Component> CreateComponent() = 0;
	
	/**
	* @brief Initialize active components
	*/
	virtual void InitComponents() = 0;

	/**
	* @brief Update active components
	*/
	virtual void UpdateComponents(std::weak_ptr<Component>& lastUpdatedComponent) = 0;

	/**
	* @brief Remove a component from the list
	*/
	virtual void RemoveComponent(const std::shared_ptr<Component>& component) = 0;

	/**
	* @brief Get the maximum number of components that can be created per list
	*/
	size_t GetMaxComponentCount() const { return m_maxComponentCount; }

	/**
	* @brief Get if the update loop is disabled for this component list
	*/
	bool IsDisabledLoop() const { return m_disabledLoop; }

	/**
	* @brief Get all components
	*/
	const std::vector<std::shared_ptr<Component>>& GetComponents() const { return shared_components; }

protected:
	std::vector<std::shared_ptr<Component>> shared_components;
	std::vector<std::shared_ptr<Component>> componentsToInit;
	size_t m_maxComponentCount;
	bool m_disabledLoop = false;
};

template<class T>
class ComponentList : public BaseComponentList
{
public:
	/**
	* @brief Constructor
	*/
	ComponentList(size_t maxComponentCount, bool disabledLoop) : BaseComponentList(maxComponentCount, disabledLoop)
	{
		ComponentData& data = componentsData.emplace_back();
		data.data = std::make_unique<uint8_t[]>(sizeof(T) * m_maxComponentCount);
		data.allocated.resize(m_maxComponentCount);
		data.remainingSlot = m_maxComponentCount;
	}

	/**
	* @brief Create a new component of the template type (Will create a new list if no slot available)
	*/
	std::shared_ptr<Component> CreateComponent()
	{
		// Find available list
		int listIndex = -1;
		const size_t listCount = componentsData.size();
		for (size_t currentListIndex = 0; currentListIndex < listCount; currentListIndex++)
		{
			if (componentsData[currentListIndex].remainingSlot != 0)
			{
				listIndex = currentListIndex;
				break;
			}
		}

		XASSERT(listIndex != -1, "No slot available for the next item");

		int addedAt = -1;
		for (size_t i = 0; i < m_maxComponentCount; ++i)
		{
			if (!componentsData[listIndex].allocated[i])
			{
				componentsData[listIndex].allocated[i] = true;
				new (&componentsData[listIndex].data[i * sizeof(T)]) T(); // Placement new
				componentsData[listIndex].remainingSlot--;
				addedAt = i;
				break;
			}
		}

		XASSERT(addedAt != -1, "No slot available for the next item");

		// Create new list if no slot available for the next item
		if (componentsData[listIndex].remainingSlot == 0)
		{
			ComponentData& data = componentsData.emplace_back();
			data.data = std::make_unique<uint8_t[]>(sizeof(T) * m_maxComponentCount);
			data.allocated.resize(m_maxComponentCount);
			data.remainingSlot = m_maxComponentCount;
		}

		std::shared_ptr<T> sharedComponent = std::shared_ptr<T>((T*)&componentsData[listIndex].data[addedAt * sizeof(T)],
			[this, addedAt, listIndex](T* pi)
			{
				pi->~T();
				if (componentsData.empty())
					return;
				componentsData[listIndex].allocated[addedAt] = false;
			});

		std::shared_ptr<Component> sharedComponentBase = std::dynamic_pointer_cast<Component>(sharedComponent);
		shared_components.push_back(sharedComponentBase);
		componentsToInit.push_back(sharedComponentBase);
		return sharedComponentBase;
	}

	/**
	* @brief Remove a component from the list
	*/
	void RemoveComponent(const std::shared_ptr<Component>& component)
	{
		const size_t sharedCount = shared_components.size();
		for (size_t i = 0; i < sharedCount; ++i)
		{
			if (shared_components[i] == component)
			{
				shared_components.erase(shared_components.begin() + i);
				break;
			}
		}
	}

	/**
	* @brief Initialize active components
	*/
	void InitComponents()
	{
		size_t componentsToInitCount = componentsToInit.size();
		for (size_t i = 0; i < componentsToInitCount; i++)
		{
			const std::shared_ptr<Component>& component = componentsToInit[i];
			if (!component->m_initiated && component->IsLocalActive() && component->IsEnabled())
			{
				component->m_initiated = true;
				component->Start();

				componentsToInit.erase(componentsToInit.begin() + i);
				i--;
				componentsToInitCount--;
			}
		}
	}

	/**
	* @brief Update active components
	*/
	void UpdateComponents(std::weak_ptr<Component>& lastUpdatedComponent)
	{
		for (const std::shared_ptr<Component>& component : shared_components)
		{
			if (component->IsLocalActive() && component->IsEnabled())
			{
#if defined(_WIN32) || defined(_WIN64)
				lastUpdatedComponent = component;
#endif
				component->Update();
			}
		}
	}

private:
	struct ComponentData
	{
		std::unique_ptr<uint8_t[]> data;
		std::vector<bool> allocated;
		size_t remainingSlot = 0;
	};
	std::vector<ComponentData> componentsData;
};

class API ComponentManager
{
public:
	static bool GetCompnentDisabledLoop(size_t typeId);

	template<typename T>
	static void AddComponentList(size_t typeId)
	{
		bool disabledLoop = GetCompnentDisabledLoop(typeId);
		componentLists[typeId] = std::make_unique<ComponentList<T>>(100, disabledLoop);
	}

	template<typename T>
	static void RemoveComponent(const std::shared_ptr<T>& component)
	{
		// Dot not use T because T is Component
		const size_t typeId = typeid(*component.get()).hash_code();
		if (componentLists.find(typeId) != componentLists.end())
		{
			componentLists[typeId]->RemoveComponent(component);
		}
	}

	template<typename T>
	static std::shared_ptr<T> CreateComponent()
	{
		// Create component list if it doesn't exist
		const size_t typeId = typeid(T).hash_code();
		if (componentLists.find(typeId) == componentLists.end())
		{
			AddComponentList<T>(typeId);
		}

		// Add component
		return std::static_pointer_cast<T>(componentLists[typeId]->CreateComponent());
	}

	static void InitComponentLists()
	{
		for (auto& componentList : componentLists)
		{
			componentList.second->InitComponents();
		}
	}

	static void UpdateComponentLists(std::weak_ptr<Component>& lastUpdatedComponent)
	{
		for (auto& componentList : componentLists)
		{
			if (componentList.second->IsDisabledLoop())
				continue;

			componentList.second->UpdateComponents(lastUpdatedComponent);
		}
	}

	static void Clear()
	{
		componentLists.clear();
	}

	static std::vector<std::shared_ptr<Component>> GetAllComponents() 
	{
		std::vector<std::shared_ptr<Component>> allComponents;
		for (auto& componentList : componentLists)
		{
			allComponents.insert(allComponents.end(), componentList.second->GetComponents().begin(), componentList.second->GetComponents().end());
		}
		return allComponents;
	}

private:
	static std::unordered_map<size_t, std::unique_ptr<BaseComponentList>> componentLists;
};
