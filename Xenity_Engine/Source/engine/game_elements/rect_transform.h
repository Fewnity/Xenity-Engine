// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/component.h>
#include <engine/math/vector2.h>

class Canvas;

/**
* @brief Component that represents a transform for UI elements
*/
class API RectTransform : public Component
{
public:

	//Vector2 anchors = Vector2(0);
	Vector2 position = Vector2(0.5f, 0.5f);

protected:
	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	friend class Canvas;

	/**
	 * @brief [Internal] Update the position of the rect transform
	 */
	void UpdatePosition(const Canvas& canvas);

	/**
	 * @brief [Internal] Update the position of the rect transform
	 */
	void UpdatePosition(const RectTransform& canvas);
};