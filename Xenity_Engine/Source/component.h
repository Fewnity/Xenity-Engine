#pragma once

#include "gameobject.h"

class Component
{
public:
	Component();
	virtual ~Component();
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	int updatePriority = 5000; //Lower is more priotary
	GameObject *gameObject = nullptr;
	bool GetIsEnabled();
	void SetIsEnabled(bool isEnabled);
	bool initiated = false;

private:
	bool isEnabled = true;
};