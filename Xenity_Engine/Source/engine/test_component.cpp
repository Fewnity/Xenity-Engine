#include "test_component.h"

TestComponent::TestComponent() 
{
	componentName = "TestComponent";
	SetReflection();
}

void TestComponent::SetReflection()
{
	reflectedVariables["myCustomClass"] = &myCustomClass;

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

CustomClass::CustomClass()
{
	SetReflection();
}


void CustomClass::SetReflection()
{
	reflectedVariables["myCustomFloat"] = &myCustomFloat;
}
