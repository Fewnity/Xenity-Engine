// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <cstdint>

#define STACK_HIGH_PRIORITY 0
#define STACK_MEDIUM_PRIORITY 1
#define STACK_LOW_PRIORITY 2
#define STACK_VERY_LOW_PRIORITY 3

#define STACK_DEBUG_LEVEL STACK_HIGH_PRIORITY

//#define ENABLE_STACK_DEBUG_OBJECT

#if defined(ENABLE_STACK_DEBUG_OBJECT)
#define STACK_DEBUG_OBJECT(level) StackDebugObject _stackDebugObject(__FUNCTION__, level)
#else
#define STACK_DEBUG_OBJECT
#endif

class StackDebugObject
{
public:
	StackDebugObject(const std::string& name, const uint32_t level);
	~StackDebugObject();

private:
	std::string m_name;
	uint32_t m_level = 0;
};

