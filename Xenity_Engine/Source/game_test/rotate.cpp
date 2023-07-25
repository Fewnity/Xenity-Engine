#include "rotate.h"
#include "../xenity.h"

Rotate::Rotate()
{
	componentName = "Rotate";
}

void Rotate::Start()
{
}

void Rotate::Update()
{
	Debug::Print("Rotate::UpdateRotate");
}

std::unordered_map<std::string, Variable> Rotate::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}

