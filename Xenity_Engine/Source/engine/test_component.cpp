#include "test_component.h"
#include "debug/debug.h"
#include "../xenity.h"
#include <memory>

TestComponent::TestComponent()
{
	componentName = "TestComponent";
	AssetManager::AddReflection(this);
	//SetReflection();
}

TestComponent::~TestComponent()
{
	AssetManager::RemoveReflection(this);
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

std::unordered_map<std::string, ReflectionEntry> TestComponent::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;

	Reflection::AddReflectionVariable(reflectedVariables, myCustomClass, "myCustomClass", true);

	Reflection::AddReflectionVariable(reflectedVariables, (std::weak_ptr<Component>&)myComponent, "myComponent", true);
	Reflection::AddReflectionVariable(reflectedVariables, myGameObject, "myGameObject", true);
	Reflection::AddReflectionVariable(reflectedVariables, myTransform, "myTransform", true);

	Reflection::AddReflectionVariable(reflectedVariables, vec2, "vec2", true);
	Reflection::AddReflectionVariable(reflectedVariables, vec2Int, "vec2Int", true);
	Reflection::AddReflectionVariable(reflectedVariables, vec3, "vec3", true);
	Reflection::AddReflectionVariable(reflectedVariables, vec3_2, "vec3_2", true);
	Reflection::AddReflectionVariable(reflectedVariables, vec3_3, "vec3_3", true);
	Reflection::AddReflectionVariable(reflectedVariables, vec4, "vec4", true);

	Reflection::AddReflectionVariable(reflectedVariables, color, "color", true);

	Reflection::AddReflectionVariable(reflectedVariables, myFloat, "myFloat", true);
	Reflection::AddReflectionVariable(reflectedVariables, myInt, "myInt", true);
	Reflection::AddReflectionVariable(reflectedVariables, myDouble, "myDouble", true);
	Reflection::AddReflectionVariable(reflectedVariables, myString, "myString", true);

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

std::unordered_map<std::string, ReflectionEntry> CustomClass::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, myCustomFloat, "myCustomFloat", true);
	Reflection::AddReflectionVariable(reflectedVariables, myCustomFloat2, "myCustomFloat2", true);
	return reflectedVariables;
}