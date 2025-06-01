// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/vectors/vector4.h>
#include <engine/reflection/reflection.h>

class API RGBA : public Reflective
{
public:
	RGBA() = delete;

	constexpr RGBA(float r, float g, float b, float a)
	{
		this->r = r;
		this->a = a;
		this->b = b;
		this->a = a;
	}

	/**
	* @brief Get RGBA as a Vector4 [0.0f;1.0f] x = red, y = green, z = blue, w = alpha
	*/
	[[nodiscard]] Vector4 ToVector4() const;

	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;

protected:
	[[nodiscard]] ReflectiveData GetReflectiveData() override;
};

class API Color : public Reflective
{
public:
	/**
	* @brief Create a color from 3 uint8_t (alpha = 255)
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	*/
	[[nodiscard]] static Color CreateFromRGB(uint8_t r, uint8_t g, uint8_t b);

	/**
	* @brief Create a color from 4 uint8_t
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	[[nodiscard]] static Color CreateFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	/**
	* @brief Create a color from 3 floats (alpha = 1)
	* @param r Red level [0.0f;1.0f]
	* @param g Green level [0.0f;1.0f]
	* @param b Blue level [0.0f;1.0f]
	*/
	[[nodiscard]] static Color CreateFromRGBFloat(float r, float g, float b);

	/**
	* @brief Create a color from 4 floats
	* @param r Red level [0.0f;1.0f]
	* @param g Green level [0.0f;1.0f]
	* @param b Blue level [0.0f;1.0f]
	* @param a Alpha level [0.0f;1.0f]
	*/
	[[nodiscard]] static Color CreateFromRGBAFloat(float r, float g, float b, float a);

	/**
	* @brief Set color from 4 uint8_t
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	void SetFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	/**
	* @brief Set color from 4 float
	* @param r Red level [0;1]
	* @param g Green level [0;1]
	* @param b Blue level [0;1]
	* @param a Alpha level [0;1]
	*/
	void SetFromRGBAFloat(float r, float g, float b, float a);

	/**
	* @brief Get RGBA
	*/
	[[nodiscard]] const RGBA& GetRGBA() const
	{
		return m_rgba;
	}

	/**
	* @brief Get RGBA values as an unsigned int in the format RGBA (0xRRGGBBAA)
	*/
	[[nodiscard]] unsigned int GetUnsignedIntRGBA() const
	{
		return m_rgbaInt;
	}

	/**
	* @brief Get RGBA values as an unsigned int in the format ABGR (0xAABBGGRR)
	*/
	[[nodiscard]] unsigned int GetUnsignedIntABGR() const
	{
		return m_abgrInt;
	}

	/**
	* @brief Get RGBA values as an unsigned int in the format ARGB (0xAARRGGBB)
	*/
	[[nodiscard]] unsigned int GetUnsignedIntARGB() const
	{
		return m_argbInt;
	}

	/**
	* @brief Return a string representation of the color
	*/
	[[nodiscard]] std::string ToString() const;

protected:

	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* @brief Update the unsigned ints
	*/
	void UpdateUnsignedInts();

	// Color informations, default is white
	RGBA m_rgba = RGBA(1, 1, 1, 1);
	unsigned int m_argbInt = 0xFFFFFFFF;
	unsigned int m_rgbaInt = 0xFFFFFFFF;
	unsigned int m_abgrInt = 0xFFFFFFFF;
};

API Color operator*(const Color& left, const Color& right);
API Color& operator*=(Color& vec, const Color& vecRight);