#include "class.h"
#include "../xenity.h"

CLASSNAME::Enemy()
{
    componentName = "CLASSNAME";
}

void CLASSNAME::Start()
{
}

void CLASSNAME::Update()
{
}

std::unordered_map<std::string, ReflectionEntry> CLASSNAME::GetReflection()
{
    std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
    // Reflection::AddVariable(reflectedVariables, variableName, "variableName", true);
    return reflectedVariables;
}
