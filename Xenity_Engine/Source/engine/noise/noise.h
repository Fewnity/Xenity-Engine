// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>

/**
* @brief Class to generate 1D, 2D, 3D and 4D Perlin noise
*/
class API Noise
{
public:

	/**
	* @brief Get 1D Perlin noise
	* @param x The x coordinate
	*/
	[[nodiscard]] static float Noise1D(float x);

	/**
	* @brief Get 2D Perlin noise
	* @param x The x coordinate
	* @param y The y coordinate
	*/
	[[nodiscard]] static float Noise2D(float x, float y);

	/**
	* @brief Get 3D Perlin noise
	* @param x The x coordinate
	* @param y The y coordinate
	* @param z The z coordinate
	*/
	[[nodiscard]] static float Noise3D(float x, float y, float z);

	/**
	* @brief Get 4D Perlin noise
	* @param x The x coordinate
	* @param y The y coordinate
	* @param z The z coordinate
	* @param w The w coordinate
	*/
	[[nodiscard]] static float Noise4D(float x, float y, float z, float w);

private:

	[[nodiscard]] static float grad1(int hash, float x);
	[[nodiscard]] static float grad2(int hash, float x, float y);
	[[nodiscard]] static float grad3(int hash, float x, float y, float z);
	[[nodiscard]] static float grad4(int hash, float x, float y, float z, float t);
	[[nodiscard]] static float pnoise1(float x, int px);
	[[nodiscard]] static float pnoise2(float x, float y, int px, int py);
	[[nodiscard]] static float pnoise3(float x, float y, float z, int px, int py, int pz);
	[[nodiscard]] static float pnoise4(float x, float y, float z, float w, int px, int py, int pz, int pw);
};