#pragma once

#include <vector>
#include <string>
#include "prop_type.h"

class Texture;

class PropData {
public:
	class Data {
	public:
		float miningRate = 1;
		Texture* texture = nullptr;
	};

	PropType type = PropType::Wood;
	std::vector<Data*> data;
	void AddPropData(float miningRate, std::string textureName);	
};