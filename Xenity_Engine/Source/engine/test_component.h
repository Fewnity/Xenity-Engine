#pragma once

#include <string>

#include "component.h"

#include "vectors/vector2.h"
#include "vectors/vector2_int.h"
#include "vectors/vector3.h"
#include "vectors/vector4.h"
#include "graphics/color/color.h"

class CustomClass : public Reflection 
{
public:
	CustomClass();
	void SetReflection();

	float myCustomFloat = 0;
};

class TestComponent : public Component
{
public:
	TestComponent();
	void SetReflection();

	CustomClass myCustomClass = CustomClass();

	Vector2 vec2 = Vector2(0);
	Vector2Int vec2Int = Vector2Int(0);
	Vector3 vec3 = Vector3(0);
	Vector4 vec4 = Vector4(0);

	Color color;

	float myFloat =0;
	int myInt =0;
	double myDouble = 0;
	std::string myString ="";
};

