#pragma once

#include "../component.h"

class API BoxCollider : public Component
{
public:
	BoxCollider();
	~BoxCollider();
	//void SetReflection();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
private:
};
