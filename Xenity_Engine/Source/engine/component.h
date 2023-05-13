#pragma once

//#include "game_elements/gameobject.h"
#include <map>
#include <string>

class GameObject;

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
	GameObject *gameObject = nullptr;
	bool GetIsEnabled();
	void SetIsEnabled(bool isEnabled);
	bool initiated = false;
	std::map<std::string, int*> reflectedInts;
	std::map<std::string, float*> reflectedFloats;
	std::string componentName = "Component";

private:
	bool isEnabled = true;
};