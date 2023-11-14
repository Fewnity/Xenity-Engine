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

std::unordered_map<std::string, ReflectionEntry> TestComponent::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;

	Reflection::AddVariable(reflectedVariables, myCustomClass, "myCustomClass", true);

	Reflection::AddVariable(reflectedVariables, myComponent, "myComponent", true);
	Reflection::AddVariable(reflectedVariables, myGameObject, "myGameObject", true);
	Reflection::AddVariable(reflectedVariables, myTransform, "myTransform", true);

	Reflection::AddVariable(reflectedVariables, vec2, "vec2", true);
	Reflection::AddVariable(reflectedVariables, vec2Int, "vec2Int", true);
	Reflection::AddVariable(reflectedVariables, vec3, "vec3", true);
	Reflection::AddVariable(reflectedVariables, vec3_2, "vec3_2", true);
	Reflection::AddVariable(reflectedVariables, vec3_3, "vec3_3", true);
	Reflection::AddVariable(reflectedVariables, vec4, "vec4", true);

	Reflection::AddVariable(reflectedVariables, color, "color", true);

	Reflection::AddVariable(reflectedVariables, myFloat, "myFloat", true);
	Reflection::AddVariable(reflectedVariables, myInt, "myInt", true);
	Reflection::AddVariable(reflectedVariables, myDouble, "myDouble", true);
	Reflection::AddVariable(reflectedVariables, myString, "myString", true);
	Reflection::AddVariable(reflectedVariables, myBool, "myBool", true);

	Reflection::AddVariable(reflectedVariables, skyBox, "skyBox", true);

	Reflection::AddVariable(reflectedVariables, textures, "textures", true);
	Reflection::AddVariable(reflectedVariables, meshData, "meshData", true);
	Reflection::AddVariable(reflectedVariables, audioClips, "audioClips", true);
	Reflection::AddVariable(reflectedVariables, scenes, "scenes", true);
	Reflection::AddVariable(reflectedVariables, skyBoxes, "skyBoxes", true);
	Reflection::AddVariable(reflectedVariables, fonts, "fonts", true);
	Reflection::AddVariable(reflectedVariables, shaders, "shaders", true);
	Reflection::AddVariable(reflectedVariables, materials, "materials", true);

	Reflection::AddVariable(reflectedVariables, myComponents, "myComponents", true);
	Reflection::AddVariable(reflectedVariables, myGameObjects, "myGameObjects", true);
	Reflection::AddVariable(reflectedVariables, myTransforms, "myTransforms", true);

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
	Reflection::AddVariable(reflectedVariables, myCustomFloat, "myCustomFloat", true);
	Reflection::AddVariable(reflectedVariables, myCustomFloat2, "myCustomFloat2", true);
	return reflectedVariables;
}