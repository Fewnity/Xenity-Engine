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
	Vector3 rotation = GetTransform()->GetRotation();
	rotation.y += Time::GetDeltaTime() * 5;
	GetTransform()->SetRotation(rotation);
}

std::unordered_map<std::string, Variable> Rotate::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}

