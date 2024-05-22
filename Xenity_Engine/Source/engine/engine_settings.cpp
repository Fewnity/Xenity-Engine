// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "engine_settings.h"
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/reflection/reflection_utils.h>

EngineSettingsValues EngineSettings::values;

void EngineSettings::SaveEngineSettings()
{
	std::shared_ptr<File> file = FileSystem::MakeFile("engine_settings.json");
	const bool result = ReflectionUtils::ReflectiveDataToFile(values.GetReflectiveData(), file);
}

void EngineSettings::LoadEngineSettings()
{
	std::shared_ptr<File> file = FileSystem::MakeFile("engine_settings.json");
	const bool result = ReflectionUtils::FileToReflectiveData(file, values.GetReflectiveData());
}

ReflectiveData EngineSettingsValues::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, useProfiler, "useProfiler", true);
	Reflective::AddVariable(reflectedVariables, useDebugger, "useDebugger", true);
	Reflective::AddVariable(reflectedVariables, useOnlineDebugger, "useOnlineDebugger", true);
	Reflective::AddVariable(reflectedVariables, compilerPath, "compilerPath", true);
	Reflective::AddVariable(reflectedVariables, ppssppExePath, "ppssppExePath", true);
	Reflective::AddVariable(reflectedVariables, dockerExePath, "dockerExePath", true);
	Reflective::AddVariable(reflectedVariables, compileOnCodeChanged, "compileOnCodeChanged", true);
	Reflective::AddVariable(reflectedVariables, compileWhenOpeningProject, "compileWhenOpeningProject", true);

	Reflective::AddVariable(reflectedVariables, backbgroundColor, "backbgroundColor", true);
	Reflective::AddVariable(reflectedVariables, secondaryColor, "secondaryColor", true);
	Reflective::AddVariable(reflectedVariables, playTintColor, "playTintColor", true);
	Reflective::AddVariable(reflectedVariables, isPlayTintAdditive, "isPlayTintAdditive", true);

	return reflectedVariables;
}