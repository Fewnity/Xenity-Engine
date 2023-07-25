#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class Rotate : public MonoBehaviour
{
public:
	Rotate();
	void Start();
	void Update();
	std::unordered_map<std::string, Variable> GetReflection();
private:
};

