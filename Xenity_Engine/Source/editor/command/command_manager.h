// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class Command;

/**
* @brief Class to manage commands (Add commands to the history to reuse them later, Undo, Redo...)
*/
class CommandManager
{
public:
	/**
	* @brief Add new command to the history, allow the command to be used by Undo and Redo. The command is executed after being added.
	* @param command Command to add
	*/
	static void AddCommandAndExecute(std::shared_ptr<Command> command);

	/**
	* @brief Clear commands history
	*/
	static void ClearCommands();

	/**
	* @brief Clear in game commands history
	*/
	static void ClearInGameCommands();

	/**
	* @brief Undo the previous command
	*/
	static void Undo();

	/**
	* @brief Redo the previous command
	*/
	static void Redo();

private:
	static void AddCommand(std::shared_ptr<Command> command);

	// Size of the command history
	static int maxCommandCount;
	// Current command index in the history
	static int currentCommand;
	// Command history
	static std::vector<std::shared_ptr<Command>> commands;
};

