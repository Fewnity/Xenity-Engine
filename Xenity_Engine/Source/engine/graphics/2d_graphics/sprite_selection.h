#pragma once

#include <engine/vectors/vector2.h>
#include <engine/reflection/reflection.h>

class SpriteSelection : Reflective
{
public:
	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, position, "position", true);
		Reflective::AddVariable(reflectedVariables, size, "size", true);
		Reflective::AddVariable(reflectedVariables, pivot, "pivot", true);
		return reflectedVariables;
	}

	Vector2 position;
	Vector2 size;
	Vector2 pivot;
};