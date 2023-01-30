#pragma once

#include "gameobject.h"

class Component
{
public:
	void SetGameObject(GameObject* gm);
	GameObject *gameObject;
};