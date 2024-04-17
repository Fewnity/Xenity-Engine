#pragma once

/**
* @brief [Internal]
*/

class Command
{
public:

	/**
	* @brief Execute the command
	*/
	virtual void Execute() = 0;

	/**
	* @brief Undo the command
	*/
	virtual void Undo() = 0;

	/**
	* @brief Redo the command
	*/
	virtual void Redo() 
	{
		Execute();
	}
};

