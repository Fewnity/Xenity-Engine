// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <cstdint>
#include <string>
#include <cassert>

/**
* @brief Function to call when an assert fails to print a message in the logs
*/
void OnAssertionFailed(const std::string& message);

// XASSERT is a macro to assert a condition and print a message in the logs in debug mode
#if defined(DEBUG)
#define XASSERT(x, message) if (!(x)) { OnAssertionFailed(message); assert(false); } 0
#else
#define XASSERT(x, message) 0
#endif

// ------------------------------- Type assertions -------------------------------
static_assert(sizeof(int) == 4, "char is not 4 byte");
static_assert(sizeof(float) == 4, "float is not 4 byte");
static_assert(sizeof(double) == 8, "double is not 8 byte");
static_assert(sizeof(long long) == 8, "long long is not 8 byte");
static_assert(sizeof(long) == 4, "long is not 4 byte");
static_assert(sizeof(short) == 2, "short is not 2 byte");
static_assert(sizeof(char) == 1, "char is not 1 byte");
static_assert(sizeof(bool) == 1, "bool is not 1 byte");
// Are these tests really necessary?
static_assert(sizeof(uint64_t) == 8, "uint64_t is not 8 byte");
static_assert(sizeof(uint32_t) == 4, "uint32_t is not 4 byte");
static_assert(sizeof(uint16_t) == 2, "uint16_t is not 2 byte");
static_assert(sizeof(int64_t) == 8, "int64_t is not 8 byte");
static_assert(sizeof(int32_t) == 4, "int32_t is not 4 byte");
static_assert(sizeof(int16_t) == 2, "int16_t is not 2 byte");