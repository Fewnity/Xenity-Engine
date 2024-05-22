// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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

