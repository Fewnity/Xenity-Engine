// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <json.hpp>

#include <engine/component.h>

/**
* @brief Component used to replace a missing component
* @brief A component is missing if the class does not exists anymore or if the game's code is not compiled
*/
class MissingScript : public Component
{
public:
	MissingScript();
	~MissingScript();

	ReflectiveData GetReflectiveData() override;
	nlohmann::json data;
};

