#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

#include "gameobject.h"

class Component
{
public:
	void SetGameObject(GameObject* gm);
	GameObject *gameObject;
};

#endif