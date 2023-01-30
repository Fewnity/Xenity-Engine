#pragma once

#include "gameobject.h"

class Component
{
public:
	Component();
	virtual ~Component();
	void SetGameObject(GameObject* gm);
	GameObject *gameObject = nullptr;
};