#pragma once

#include "gameobject.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour()
	{
		this->updatePriority = 1000;
	}
};