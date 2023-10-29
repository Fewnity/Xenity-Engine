#include "{FILENAME}.h"
#include "../xenity.h"

/**
* Constructor
*/
{CLASSNAME}::{CLASSNAME}()
{
    // Register class name
    componentName = "{CLASSNAME}";
}

/**
* Called once when the script is enabled
*/
void {CLASSNAME}::Start()
{
}

/**
* Called every frame
*/
void {CLASSNAME}::Update()
{
}

/**
* Lists all variables to show in inspector
*/
std::unordered_map<std::string, ReflectionEntry> {CLASSNAME}::GetReflection()
{
    std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
    // Reflection::AddVariable(reflectedVariables, variableName, "variableName", true);
    return reflectedVariables;
}
