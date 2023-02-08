#pragma once

#include "gameobject.h"

class Component
{
public:
	Component();
	virtual ~Component();
	virtual void Update();
	GameObject *gameObject = nullptr;
};