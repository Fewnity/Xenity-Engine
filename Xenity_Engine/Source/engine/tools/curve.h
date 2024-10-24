// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <memory>

#include <engine/api.h>
#include <engine/vectors/vector3.h>

class Transform;
class GameObject;

class API SplinePoint
{
public:
	std::weak_ptr<Transform> parent;
	std::weak_ptr<Transform> before;
	std::weak_ptr<Transform> next;
};

class API Spline
{
public:
	/**
	* @brief Create a spline point
	* @param position Position of the point
	*/
	SplinePoint* CreateSplinePoint(const Vector3& position);

	/**
	* @brief Add a point to the spline
	* @param point Point of the spline
	*/
	void AddSplinePoint(SplinePoint* point);

	/**
	* @brief Get spline value at t
	* @param t [0;1]
	*/
	[[nodiscard]] Vector3 GetValueAt(const float t) const;
private:
	std::vector<SplinePoint*> splinePoints;
};
