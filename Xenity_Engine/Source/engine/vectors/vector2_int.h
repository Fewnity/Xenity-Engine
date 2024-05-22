// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/reflection/reflection.h>

/*
* @brief Contains integer 2D coordinates 
*/
class API Vector2Int : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	Vector2Int();
	Vector2Int(const int x, const int y);
	Vector2Int(const int fillValue);

	/**
	* @brief Linearly interpolates between vectors
	*/
	static float Distance(const Vector2Int& a, const Vector2Int& b);

	/**
	* @brief Get the length of this vector
	*/
	float Magnitude() const;

	int x;
	int y;
};


API Vector2Int operator+(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator-(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator*(const Vector2Int& vec, const float value);
API Vector2Int operator*(const float value, const Vector2Int& vec);
API Vector2Int operator*(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator/(const Vector2Int& left, const Vector2Int& right);
API Vector2Int operator/(const Vector2Int& vec, const float value);
API Vector2Int operator/(const float value, const Vector2Int& vec);

API Vector2Int operator/=(Vector2Int& vec, const float value);
API Vector2Int operator*=(Vector2Int& vec, const float value);
API Vector2Int operator+=(Vector2Int& vec, const float value);
API Vector2Int operator-=(Vector2Int& vec, const float value);

API Vector2Int operator/=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator*=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator+=(Vector2Int& vec, const Vector2Int& vecRight);
API Vector2Int operator-=(Vector2Int& vec, const Vector2Int& vecRight);

API bool operator==(const Vector2Int& left, const Vector2Int& right);
API bool operator!=(const Vector2Int& left, const Vector2Int& right);