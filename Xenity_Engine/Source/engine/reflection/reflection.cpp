// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "reflection.h"

void Reflective::CreateReflectionEntry(ReflectiveData& vector, const VariableReference& variable, const std::string& variableName, const bool visibleInFileInspector, const bool isPublic, const uint64_t id, const bool isEnum)
{
	ReflectiveEntry entry;
	entry.variable = variable;
	entry.visibleInFileInspector = visibleInFileInspector;
	entry.isPublic = isPublic;
	entry.typeId = id;
	entry.isEnum = isEnum;
	
	vector.push_back(std::make_pair(variableName, entry));
}