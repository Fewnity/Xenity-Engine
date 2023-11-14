#include "command_manager.h"

std::vector<std::shared_ptr<Command>> CommandManager::commands;
int CommandManager::maxCommandCount = 128;
int CommandManager::currentCommand = -1;

void CommandManager::AddCommand(std::shared_ptr<Command> command)
{
	commands.push_back(command);
	if (commands.size() >= maxCommandCount)
	{
		commands.erase(commands.begin());
		commands.push_back(command);
	}else
	{
		currentCommand++;
	}
}

void CommandManager::ClearCommands()
{
	commands.clear();
}

void CommandManager::Undo()
{
	if (currentCommand >= 0) 
	{
		commands[currentCommand]->Undo();
		currentCommand--;
	}
}

void CommandManager::Redo()
{
	//commands[currentCommand]->Redo();
	//currentCommand++;
}
