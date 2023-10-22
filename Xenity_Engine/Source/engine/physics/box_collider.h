#pragma once

#include "collider.h"

class API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();
	//void SetReflection();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
private:
};
