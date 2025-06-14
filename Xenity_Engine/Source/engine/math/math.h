// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>

class API Math
{
public:
	static constexpr float PI = 3.14159265359f;

	/**
	* @brief Linearly interpolates between a and b by t
	* @param a Start value
	* @param b End value
	* @param t [0,1]
	*/
	[[nodiscard]] static float Lerp(float a, float b, float t);

	/**
	* @brief Restrict a number between two other numbers
	* @param value Value to clamp
	* @param min Minimum
	* @param mac Maximum
	*/
	[[nodiscard]] static float Clamp(float value, float min, float max);
};