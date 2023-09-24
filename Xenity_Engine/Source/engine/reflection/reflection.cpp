#include "reflection.h"
#include "../../xenity.h"

void Reflection::AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry>& map, const Variable& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic)
{
	ReflectionEntry entry;
	entry.variable = variable;
	entry.visibleInFileInspector = visibleInFileInspector;
	entry.isPublic = isPublic;
	map[variableName] = entry;
}

void Reflection::AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry>& map, const Variable& variable, const std::string& variableName, bool isPublic)
{
	AddReflectionVariable(map, variable, variableName, false, isPublic);
}
