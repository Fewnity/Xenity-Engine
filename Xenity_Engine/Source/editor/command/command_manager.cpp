// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "command_manager.h"

#include <editor/command/command.h>

#include <engine/assertions/assertions.h>
#include <engine/game_elements/gameplay_manager.h>

std::vector<std::shared_ptr<Command>> CommandManager::commands;
int CommandManager::maxCommandCount = 100;
int CommandManager::currentCommand = -1;

void CommandManager::AddCommand(std::shared_ptr<Command> command)
{
	XASSERT(command != nullptr, "[CommandManager::AddCommand] command is nullptr");

	commands.push_back(command);
	size_t commandCount = commands.size();
	// If we are not at the end of the list, remove all other commands starting from currentCommand to the end of the list
	if (currentCommand != commandCount - 1)
	{
		const int count = (commandCount - 1) - (currentCommand + 1);
		for (int i = 0; i < count; i++)
		{
			commands.erase(commands.begin() + (currentCommand + 1));
			commandCount--;
		}
	}

	// If the history is full, 
	if (commandCount >= maxCommandCount)
	{
		commands.erase(commands.begin());
	}
	else
	{
		currentCommand++;
	}
}

void CommandManager::AddCommandAndExecute(std::shared_ptr<Command> command)
{
	XASSERT(command != nullptr, "[CommandManager::AddCommand] command is nullptr");

	AddCommand(command);
	command->Execute();
}

void CommandManager::ClearCommands()
{
	commands.clear();
}

void CommandManager::ClearInGameCommands()
{
	size_t commandCount = commands.size();
	for (size_t i = 0; i < commandCount; i++)
	{
		if (commands[i]->doneInPlayMode) 
		{
			commands.erase(commands.begin() + i);
			commandCount--;
			i--;
		}
	}
	currentCommand = commandCount - 1;
}

void CommandManager::Undo()
{
	// If we are not at the beginning of the list
	if (currentCommand >= 0)
	{
		commands[currentCommand]->Undo();
		currentCommand--;
	}
}

void CommandManager::Redo()
{
	//If we are not at the end of the list
	if ((int)commands.size() - 1 > currentCommand)
	{
		currentCommand++;
		commands[currentCommand]->Redo();
	}
}
