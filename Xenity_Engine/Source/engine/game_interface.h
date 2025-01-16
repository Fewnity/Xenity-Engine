// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
* @brief Interface used to interactor with the compiled game class
*/
class GameInterface
{
public:
	virtual ~GameInterface() = default;

	/**
	* @brief Start game by registering game's components
	*/
	virtual void Start() = 0;
};