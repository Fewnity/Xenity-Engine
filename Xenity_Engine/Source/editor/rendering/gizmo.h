// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>

class Vector3;
class Vector2;
class Quaternion;
class Color;
class Texture;

class API Gizmo
{
public:

	/**
	* @brief [Internal] Init Gizmo system
	*/
	static void Init();

	/**
	* @brief Draw a simple line From A to B
	* @param a Start point
	* @param b End point
	*/
	static void DrawLine(const Vector3& a, const Vector3& b);

	/**
	* @brief Set Gizmo draw color
	* @param newColor New color to set
	*/
	static void SetColor(const Color& newColor);

	/**
	* @brief Draw a billboard sprite
	* @param position Position
	* @param scale Scale
	* @param texture Texture to draw
	* @param color Color
	*/
	static void DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color);

	static void DrawSphere(const Vector3& position, const Quaternion& rotation, const float radius);

private:
	static Color color;
};

