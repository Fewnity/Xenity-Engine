#include "reflection.h"

void Reflective::AddReflectionVariable(ReflectiveData& map, const VariableReference& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic, uint64_t id)
{
	ReflectiveEntry entry;
	entry.variable = variable;
	entry.visibleInFileInspector = visibleInFileInspector;
	entry.isPublic = isPublic;
	entry.typeId = id;
	map[variableName] = entry;
}