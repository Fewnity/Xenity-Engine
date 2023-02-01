#pragma once

#include "gameobject.h"

class Component
{
public:
	Component();
	virtual ~Component();
	virtual void Update();
	void SetParent(GameObject* gm);
	GameObject *gameObject = nullptr;
};