#pragma once

#include <vector>
#include <memory>

class Command;

class CommandManager
{
public:
	static void AddCommand(std::shared_ptr<Command> command);
	static void ClearCommands();
	static void Undo();
	static void Redo();
	static int maxCommandCount;
	static int currentCommand;
private:
	static std::vector<std::shared_ptr<Command>> commands;
};

