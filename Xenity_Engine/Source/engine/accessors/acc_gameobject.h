#pragma once

#include <memory>
#include <vector>

class GameObject;
class Component;

/**
* @brief class to access GameObject private members
*/
class GameObjectAccessor
{
public:
	GameObjectAccessor() = delete;
	explicit GameObjectAccessor(const std::shared_ptr<GameObject>& gameObject) 
	{
		this->gameObject = gameObject;
	}

	std::vector<std::shared_ptr<Component>>& GetComponents();
	std::vector<std::weak_ptr<GameObject>>& GetChildren();
	void RemoveComponent(const std::shared_ptr <Component>& component);
	void Setup();
	bool IsWaitingForDestroy();
	void SetWaitingForDestroy(bool waitingForDestroy);
	void SetChildrenCount(int count);

private:
	std::shared_ptr<GameObject> gameObject;
};

