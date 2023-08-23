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
	reflectedVariables.insert_or_assign("myCustomClass", myCustomClass);

	reflectedVariables.insert_or_assign("myComponent", (std::weak_ptr<Component>&)myComponent);
	reflectedVariables.insert_or_assign("myGameObject", myGameObject);
	reflectedVariables.insert_or_assign("myTransform", myTransform);

	reflectedVariables.insert_or_assign("vec2", vec2);
	reflectedVariables.insert_or_assign("vec2Int", vec2Int);
	reflectedVariables.insert_or_assign("vec3", vec3);
	reflectedVariables.insert_or_assign("vec3_2", vec3_2);
	reflectedVariables.insert_or_assign("vec3_3", vec3_3);
	reflectedVariables.insert_or_assign("vec4", vec4);

	reflectedVariables.insert_or_assign("color", color);

	reflectedVariables.insert_or_assign("myFloat", myFloat);
	reflectedVariables.insert_or_assign("myInt", myInt);
	reflectedVariables.insert_or_assign("myDouble", myDouble);
	reflectedVariables.insert_or_assign("myString", myString);
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
	reflectedVariables.insert_or_assign("myCustomFloat", myCustomFloat);
	reflectedVariables.insert_or_assign("myCustomFloat2", myCustomFloat2);
	return reflectedVariables;
}