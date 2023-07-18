#include "test_component.h"
#include "debug/debug.h"
#include "../xenity.h"
#include <memory>

TestComponent::TestComponent()
{
	componentName = "TestComponent";
	//SetReflection();
}

/*void TestComponent::SetReflection()
{
	reflectedVariables["myCustomClass"] = myCustomClass;

	reflectedVariables["myComponent"] = &myComponent;
	reflectedVariables["myGameObject"] = &myGameObject;
	reflectedVariables["myTransform"] = &myTransform;

	reflectedVariables["vec2"] = vec2;
	reflectedVariables["vec2Int"] = vec2Int;
	reflectedVariables["vec3"] = vec3;
	reflectedVariables["vec3_2"] = vec3_2;
	reflectedVariables["vec3_3"] = vec3_3;
	reflectedVariables["vec4"] = vec4;

	reflectedVariables["color"] = color;

	reflectedVariables["myFloat"] = myFloat;
	reflectedVariables["myInt"] = &myInt;
	reflectedVariables["myDouble"] = &myDouble;
	reflectedVariables["myString"] = &myString;
}*/

std::unordered_map<std::string, Variable> TestComponent::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables["myCustomClass"] = myCustomClass;

	reflectedVariables["myComponent"] = &myComponent;
	reflectedVariables["myGameObject"] = &myGameObject;
	reflectedVariables["myTransform"] = &myTransform;

	reflectedVariables["vec2"] = vec2;
	reflectedVariables["vec2Int"] = vec2Int;
	reflectedVariables["vec3"] = vec3;
	reflectedVariables["vec3_2"] = vec3_2;
	reflectedVariables["vec3_3"] = vec3_3;
	reflectedVariables["vec4"] = vec4;

	reflectedVariables["color"] = color;

	reflectedVariables["myFloat"] = myFloat;
	reflectedVariables["myInt"] = myInt;
	reflectedVariables["myDouble"] = myDouble;
	reflectedVariables["myString"] = myString;
	return reflectedVariables;
}

void TestComponent::Start()
{
}

void TestComponent::Update()
{

}

CustomClass::CustomClass()
{
}


/*void CustomClass::SetReflection()
{
	reflectedVariables["myCustomFloat"] = myCustomFloat;
}*/

std::unordered_map<std::string, Variable> CustomClass::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables["myCustomFloat"] = myCustomFloat;
	return reflectedVariables;
}