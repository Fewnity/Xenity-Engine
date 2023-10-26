#include "reflection.h"
#include "../../xenity.h"

void Reflection::AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry>& map, const Variable& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic, uint64_t id)
{
	ReflectionEntry entry;
	entry.variable = variable;
	entry.visibleInFileInspector = visibleInFileInspector;
	entry.isPublic = isPublic;
	entry.typeId = id;
	map[variableName] = entry;
}