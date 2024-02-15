#include "reflection.h"

void Reflective::AddReflectionVariable(ReflectiveData& map, const VariableReference& variable, const std::string& variableName, const bool visibleInFileInspector, const bool isPublic, const uint64_t id, const bool isEnum)
{
	ReflectiveEntry entry;
	entry.variable = variable;
	entry.visibleInFileInspector = visibleInFileInspector;
	entry.isPublic = isPublic;
	entry.typeId = id;
	entry.isEnum = isEnum;
	map[variableName] = entry;
}