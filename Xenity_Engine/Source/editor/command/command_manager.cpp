#include "command_manager.h"

std::vector<std::shared_ptr<Command>> CommandManager::commands;
int CommandManager::maxCommandCount = 128;
int CommandManager::currentCommand = -1;

void CommandManager::AddCommand(std::shared_ptr<Command> command)
{
	commands.push_back(command);
	if (currentCommand != commands.size() - 1) 
	{
		int count = (commands.size() -1) - (currentCommand + 1);
		for (int i = 0; i < count; i++)
		{
			commands.erase(commands.begin() + currentCommand + 1);
		}
	}
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
	if (currentCommand >= 0)
	{
		commands[currentCommand]->Undo();
		currentCommand--;
	}
}

void CommandManager::Redo()
{
	if ((int)commands.size() - 1 > currentCommand)
	{
		currentCommand++;
		commands[currentCommand]->Redo();
	}
}
