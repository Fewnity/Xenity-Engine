// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "command_manager.h"

#include <editor/command/command.h>

#include <engine/assertions/assertions.h>
#include <engine/game_elements/gameplay_manager.h>

std::vector<std::shared_ptr<Command>> CommandManager::s_commands;
int CommandManager::s_maxCommandCount = 100;
int CommandManager::s_currentCommand = -1;

void CommandManager::AddCommand(std::shared_ptr<Command> command)
{
	XASSERT(command != nullptr, "[CommandManager::AddCommand] command is nullptr");

	s_commands.push_back(command);
	size_t commandCount = s_commands.size();
	// If we are not at the end of the list, remove all other commands starting from currentCommand to the end of the list
	if (s_currentCommand != commandCount - 1)
	{
		const int count = static_cast<int>((commandCount - 1) - (s_currentCommand + 1));
		for (int i = 0; i < count; i++)
		{
			s_commands.erase(s_commands.begin() + (s_currentCommand + 1));
			commandCount--;
		}
	}

	// If the history is full, 
	if (commandCount >= s_maxCommandCount)
	{
		s_commands.erase(s_commands.begin());
	}
	else
	{
		s_currentCommand++;
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
	s_commands.clear();
	s_currentCommand = -1;
}

void CommandManager::ClearInGameCommands()
{
	// Keep the cursor on the same kept command, so commands undone before playing stay undone
	std::vector<std::shared_ptr<Command>> keptCommands;
	int newCurrentCommand = -1;
	const size_t commandCount = s_commands.size();
	for (size_t i = 0; i < commandCount; i++)
	{
		if (s_commands[i]->doneInPlayMode)
			continue;

		keptCommands.push_back(s_commands[i]);
		if (static_cast<int>(i) <= s_currentCommand)
		{
			newCurrentCommand = static_cast<int>(keptCommands.size()) - 1;
		}
	}
	s_commands = std::move(keptCommands);
	s_currentCommand = newCurrentCommand;
}

void CommandManager::Undo()
{
	// If we are not at the beginning of the list
	if (s_currentCommand >= 0)
	{
		s_commands[s_currentCommand]->Undo();
		s_currentCommand--;
	}
}

void CommandManager::Redo()
{
	//If we are not at the end of the list
	if ((int)s_commands.size() - 1 > s_currentCommand)
	{
		s_currentCommand++;
		s_commands[s_currentCommand]->Redo();
	}
}
