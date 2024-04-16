#pragma once

/**
* [Internal]
*/

class Command
{
public:

	/**
	* Execute the command
	*/
	virtual void Execute() = 0;

	/**
	* Undo the command
	*/
	virtual void Undo() = 0;

	/**
	* Redo the command
	*/
	virtual void Redo() 
	{
		Execute();
	}
};

