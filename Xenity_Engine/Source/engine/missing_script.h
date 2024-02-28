#pragma once

#include <json.hpp>
#include <engine/component.h>

class MissingScript : public Component
{
public:
	MissingScript();
	~MissingScript();

	ReflectiveData GetReflectiveData() override;
	nlohmann::json data;
};

