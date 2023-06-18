#pragma once

//#include "game_elements/gameobject.h"
#include <map>
#include <string>
#include <memory>

class GameObject;
class Transform;

class Component
{
public:
	Component();

	virtual ~Component();
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	//virtual void OnParentChanged() {}

	int updatePriority = 5000; //Lower is more priotary
	bool GetIsEnabled();
	void SetIsEnabled(bool isEnabled);
	bool initiated = false;
	std::map<std::string, int*> reflectedInts;
	std::map<std::string, float*> reflectedFloats;
	std::string componentName = "Component";

	bool waitingForDestroy = false;

	void SetGameObject(std::weak_ptr<GameObject> go);

	std::weak_ptr <GameObject> GetGameObject() const
	{
		return gameObject;
	}

	std::weak_ptr <Transform> GetTransform() const
	{
		return transform;
	}

private:
	std::weak_ptr <GameObject>gameObject;
	std::weak_ptr <Transform> transform;
	bool isEnabled = true;
};