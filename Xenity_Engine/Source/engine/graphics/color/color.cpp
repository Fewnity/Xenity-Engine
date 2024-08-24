// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "color.h"

#include <math.h>
#include <sstream>

#include <engine/tools/math.h>

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

std::string Color::ToString() const
{
	std::stringstream stream;
	stream << "{";
	stream << "R:" << m_rgba.r;
	stream << " G:" << m_rgba.g;
	stream << " B:" << m_rgba.b;
	stream << " A:" << m_rgba.a;
	stream << " | RGBA Hex:#" << std::hex << m_rgbaInt;
	stream << " | ABGR Hex:#" << std::hex << m_abgrInt;
	stream << "}";
	std::string result(stream.str());

	return result;
}

ReflectiveData Color::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_rgba, "rgba", true);
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
	m_rgba.r = Math::Clamp(r / 255.0f, 0, 1);
	m_rgba.g = Math::Clamp(g / 255.0f, 0, 1);
	m_rgba.b = Math::Clamp(b / 255.0f, 0, 1);
	m_rgba.a = Math::Clamp(a / 255.0f, 0, 1);
	m_rgbaInt = ((int)(m_rgba.r * 255) << 24) + ((int)(m_rgba.g * 255) << 16) + ((int)(m_rgba.b * 255) << 8) + ((int)(m_rgba.a * 255) << 0);
	m_abgrInt = ((int)(m_rgba.a * 255) << 24) + ((int)(m_rgba.b * 255) << 16) + ((int)(m_rgba.g * 255) << 8) + ((int)(m_rgba.r * 255) << 0);
}

void Color::SetFromRGBAfloat(float r, float g, float b, float a)
{
	m_rgba.r = Math::Clamp(r, 0, 1);
	m_rgba.g = Math::Clamp(g, 0, 1);
	m_rgba.b = Math::Clamp(b, 0, 1);
	m_rgba.a = Math::Clamp(a, 0, 1);
	m_rgbaInt = ((int)(m_rgba.r * 255) << 24) + ((int)(m_rgba.g * 255) << 16) + ((int)(m_rgba.b * 255) << 8) + ((int)(m_rgba.a * 255) << 0);
	m_abgrInt = ((int)(m_rgba.a * 255) << 24) + ((int)(m_rgba.b * 255) << 16) + ((int)(m_rgba.g * 255) << 8) + ((int)(m_rgba.r * 255) << 0);
}

void Color::UpdateUnsignedInts()
{
	m_rgbaInt = ((int)(m_rgba.r * 255) << 24) + ((int)(m_rgba.g * 255) << 16) + ((int)(m_rgba.b * 255) << 8) + ((int)(m_rgba.a * 255) << 0);
	m_abgrInt = ((int)(m_rgba.a * 255) << 24) + ((int)(m_rgba.b * 255) << 16) + ((int)(m_rgba.g * 255) << 8) + ((int)(m_rgba.r * 255) << 0);
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

Color operator*(const Color& left, const Color& right)
{
	const Vector4 colorMix = right.GetRGBA().ToVector4() * left.GetRGBA().ToVector4();

	Color newColor = Color::CreateFromRGBAFloat(colorMix.x, colorMix.y, colorMix.z, colorMix.w);

	return newColor;
}

Color& operator*=(Color& color, const Color& colorRight)
{
	const Vector4 colorMix = color.GetRGBA().ToVector4() * colorRight.GetRGBA().ToVector4();

	color.SetFromRGBAfloat(colorMix.x, colorMix.y, colorMix.z, colorMix.w);

	return color;
}
