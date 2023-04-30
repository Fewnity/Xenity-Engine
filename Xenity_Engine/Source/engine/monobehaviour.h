#pragma once

#include "game_elements/gameobject.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour()
	{
		this->updatePriority = 1000;
	}
};