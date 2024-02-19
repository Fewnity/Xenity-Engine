#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class Command;

/**
* Class to manage commands (Add commands to the history to reuse them later, Undo, Redo...)
*/
class CommandManager
{
public:
	/**
	* Add new command to the history, allow the command to be used by Undo and Redo
	*/
	static void AddCommand(std::shared_ptr<Command> command);

	/**
	* Clear commands history
	*/
	static void ClearCommands();

	/**
	* Undo the previous command
	*/
	static void Undo();

	/**
	* Redo the previous command
	*/
	static void Redo();

private:
	// Size of the command history
	static int maxCommandCount;
	// Current command index in the history
	static int currentCommand;
	// Command history
	static std::vector<std::shared_ptr<Command>> commands;
};

