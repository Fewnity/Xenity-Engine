// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "missing_script.h"

#include "asset_management/asset_manager.h"

ReflectiveData MissingScript::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, data, "data", false);
	return reflectedVariables;
}
