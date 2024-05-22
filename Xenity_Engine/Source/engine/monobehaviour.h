// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include "component.h"

/**
* @brief Add this to a class to act like a component
*/
class API MonoBehaviour : public Component
{
public:
	MonoBehaviour();
};