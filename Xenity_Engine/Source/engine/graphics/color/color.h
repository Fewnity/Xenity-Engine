// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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
	Vector4 ToVector4() const;

	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;

protected:
	ReflectiveData GetReflectiveData() override;
};

class API Color : public Reflective
{
public:
	Color();

	/**
	* @brief Create color from ints
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	*/
	static Color CreateFromRGB(int r, int g, int b);

	/**
	* @brief Create color from floats
	* @param r Red level [0.0f;1.0f]
	* @param g Green level [0.0f;1.0f]
	* @param b Blue level [0.0f;1.0f]
	*/
	static Color CreateFromRGBFloat(float r, float g, float b);

	/**
	* @brief Create color with alpha information from ints
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	static Color CreateFromRGBA(int r, int g, int b, int a);

	/**
	* @brief Create color from floats
	* @param r Red level [0.0f;1.0f]
	* @param g Green level [0.0f;1.0f]
	* @param b Blue level [0.0f;1.0f]
	* @param a Alpha level [0.0f;1.0f]
	*/
	static Color CreateFromRGBAFloat(float r, float g, float b, float a);

	/**
	* @brief Set color with alpha information from ints
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	void SetFromRGBA(int r, int g, int b, int a);

	/**
	* @brief Set color from with alpha information floats
	* @param r Red level [0;1]
	* @param g Green level [0;1]
	* @param b Blue level [0;1]
	* @param a Alpha level [0;1]
	*/
	void SetFromRGBAfloat(float r, float g, float b, float a);

	/**
	* @brief Get RGBA
	*/
	inline const RGBA& GetRGBA() const
	{
		return rgba;
	}

	/**
	* @brief Get RGBA value as an unsigned int
	*/
	inline unsigned int GetUnsignedIntRGBA() const
	{
		return rgbaInt;
	}

	/**
	* @brief Get ABGR value as an unsigned int
	*/
	inline unsigned int GetUnsignedIntABGR() const
	{
		return abgrInt;
	}

	/**
	* @brief Return a string representation of the color
	*/
	std::string ToString() const;

protected:

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* @brief Update the unsigned ints
	*/
	void UpdateUnsignedInts();

	// Color informations, default is white
	RGBA rgba = RGBA(1, 1, 1, 1);
	unsigned int rgbaInt = 0xFFFFFFFF;
	unsigned int abgrInt = 0xFFFFFFFF;
};

API Color operator*(const Color& left, const Color& right);
API Color& operator*=(Color& vec, const Color& vecRight);