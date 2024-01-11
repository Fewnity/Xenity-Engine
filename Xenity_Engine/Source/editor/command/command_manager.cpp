#include "command_manager.h"

#include <editor/command/command.h>

std::vector<std::shared_ptr<Command>> CommandManager::commands;
int CommandManager::maxCommandCount = 128;
int CommandManager::currentCommand = -1;

void CommandManager::AddCommand(std::shared_ptr<Command> command)
{
	commands.push_back(command);
	// If we are not at the end of the list, remove all other commands starting from currentCommand to the end of the list
	if (currentCommand != commands.size() - 1) 
	{
		const int count = (commands.size() - 1) - (currentCommand + 1);
		for (int i = 0; i < count; i++)
		{
			commands.erase(commands.begin() + (currentCommand + 1));
		}
	}
	
	// 
	if (commands.size() >= maxCommandCount)
	{
		commands.erase(commands.begin());
		commands.push_back(command);
	}
	else
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
