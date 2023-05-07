#pragma once

#include "prop_type.h"
class PropData;

class Prop 
{
public:
	int id = 0;
	int x = 0;
	int y = 0;
	//PropType type = PropType::Wood;
	PropData* data = nullptr;

	void SetPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};