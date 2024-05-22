// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "color.h"
#include <engine/tools/math.h>
#include <math.h>

/**
 * @brief Create color from RGB
 *
 * @param r Red [0;255]
 * @param g Green [0;255]
 * @param b Blue [0;255]
 * @return Color
 */

Color::Color()
{
}

ReflectiveData Color::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rgba, "rgba", true);
	return reflectedVariables;
}

void Color::OnReflectionUpdated()
{
	Color::UpdateUnsignedInts();
}

Color Color::CreateFromRGB(int r, int g, int b)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, 255);
	return color;
}

Color Color::CreateFromRGBFloat(float r, float g, float b)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, 1.0f);
	return color;
}

Color Color::CreateFromRGBA(int r, int g, int b, int a)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, a);
	return color;
}

Color Color::CreateFromRGBAFloat(float r, float g, float b, float a)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, a);
	return color;
}

void Color::SetFromRGBA(int r, int g, int b, int a)
{
	rgba.r = Math::Clamp(r / 255.0f, 0, 1);
	rgba.g = Math::Clamp(g / 255.0f, 0, 1);
	rgba.b = Math::Clamp(b / 255.0f, 0, 1);
	rgba.a = Math::Clamp(a / 255.0f, 0, 1);
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

void Color::SetFromRGBAfloat(float r, float g, float b, float a)
{
	rgba.r = Math::Clamp(r, 0, 1);
	rgba.g = Math::Clamp(g, 0, 1);
	rgba.b = Math::Clamp(b, 0, 1);
	rgba.a = Math::Clamp(a, 0, 1);
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

void Color::UpdateUnsignedInts()
{
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

ReflectiveData RGBA::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, r, "r", true);
	Reflective::AddVariable(reflectedVariables, g, "g", true);
	Reflective::AddVariable(reflectedVariables, b, "b", true);
	Reflective::AddVariable(reflectedVariables, a, "a", true);
	return reflectedVariables;
}

Vector4 RGBA::ToVector4() const
{
	return Vector4(r, g, b, a);
}