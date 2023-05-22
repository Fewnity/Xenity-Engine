#pragma once

#include <vector>
#include <string>
#include "prop_type_enum.h"

class Texture;

class PropData 
{
public:
	class Data 
	{
	public:
		float miningRate = 1;
		Texture* texture = nullptr;
	};

	void AddPropData(float miningRate, std::string textureName);	

	PropType type = PropType::Wood;
	std::vector<Data*> data;
};