#include "test_component.h"
#include "debug/debug.h"
#include "../xenity.h"
#include <memory>

TestComponent::TestComponent() 
{
	componentName = "TestComponent";
	SetReflection();
}

void TestComponent::SetReflection()
{
	reflectedVariables["myCustomClass"] = &myCustomClass;

	reflectedVariables["myComponent"] = &myComponent;
	reflectedVariables["myGameObject"] = &myGameObject;
	reflectedVariables["myTransform"] = &myTransform;

	reflectedVariables["vec2"] = &vec2;
	reflectedVariables["vec2Int"] = &vec2Int;
	reflectedVariables["vec3"] = &vec3;
	reflectedVariables["vec4"] = &vec4;

	reflectedVariables["color"] = &color;

	reflectedVariables["myFloat"] = &myFloat;
	reflectedVariables["myInt"] = &myInt;
	reflectedVariables["myDouble"] = &myDouble;
	reflectedVariables["myString"] = &myString;
}

void TestComponent::Start()
{
}

void TestComponent::Update()
{
}

CustomClass::CustomClass()
{
	SetReflection();
}


void CustomClass::SetReflection()
{
	reflectedVariables["myCustomFloat"] = &myCustomFloat;
}
