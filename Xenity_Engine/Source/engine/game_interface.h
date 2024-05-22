// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
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
	* @brief Start game by registering game's monobehaviours
	*/
	virtual void Start() = 0;
};