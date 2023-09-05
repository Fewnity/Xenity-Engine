#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class FreeMove : public MonoBehaviour
{
public:
	FreeMove();
	void Start();
	void Update();
	std::unordered_map<std::string, ReflectionEntry> GetReflection();
private:
};

