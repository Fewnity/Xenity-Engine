// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/component.h>
#include <engine/math/vector2.h>

class Canvas;

class RectTransform : public Component
{
public:
	RectTransform();
	~RectTransform();

	Vector2 anchors = Vector2(0);
	Vector2 position = Vector2(0);

protected:
	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	friend class Canvas;

	/**
	 * @brief [Internal] Update the position of the rect transform
	 */
	void UpdatePosition(const std::shared_ptr <Canvas>& canvas);

	/**
	 * @brief [Internal] Update the position of the rect transform
	 */
	void UpdatePosition(const std::shared_ptr <RectTransform>& canvas);
};