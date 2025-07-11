// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
	Reflective::AddVariable(reflectedVariables, useProfiler, "useProfiler");
	Reflective::AddVariable(reflectedVariables, useDebugger, "useDebugger");
	Reflective::AddVariable(reflectedVariables, useOnlineDebugger, "useOnlineDebugger");
	Reflective::AddVariable(reflectedVariables, compilerPath, "compilerPath");
	Reflective::AddVariable(reflectedVariables, ppssppExePath, "ppssppExePath");
	Reflective::AddVariable(reflectedVariables, dockerExePath, "dockerExePath");
	Reflective::AddVariable(reflectedVariables, ps3CtrlPath, "ps3CtrlPath");
	Reflective::AddVariable(reflectedVariables, compileOnCodeChanged, "compileOnCodeChanged");
	Reflective::AddVariable(reflectedVariables, compileWhenOpeningProject, "compileWhenOpeningProject");

	Reflective::AddVariable(reflectedVariables, backbgroundColor, "backbgroundColor");
	Reflective::AddVariable(reflectedVariables, secondaryColor, "secondaryColor");
	Reflective::AddVariable(reflectedVariables, playTintColor, "playTintColor");
	Reflective::AddVariable(reflectedVariables, isPlayTintAdditive, "isPlayTintAdditive");

	return reflectedVariables;
}