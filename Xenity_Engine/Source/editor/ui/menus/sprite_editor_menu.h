// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include "menu.h"

#include <engine/vectors/vector2.h>
#include <engine/graphics/2d_graphics/sprite_selection.h>

class Texture;


class SpriteEditorMenu : public Menu
{
public:	
	void Init() override;
	void Draw() override;

private:

	/**
	* Load sprite selections from the current sprite sheet
	*/
	void LoadSpriteSelections();

	/**
	* Save sprite selections of the current sprite sheet
	*/
	void SaveSpriteSelections();

	/**
	* Draw sprite sheet overlay menu
	*/
	void DrawSpriteSheetOverlay();

	/**
	* Draw sprite sheet image and selections lines
	*/
	void DrawSpriteSheet();

	/**
	* Draw the sprite editor tool window
	*/
	void DrawToolWindow();

	/**
	* Detect mouse and move the sprite sheet
	*/
	void MoveSpriteSheet();

	/**
	* Add a new sprite selection
	*/
	void AddNewSpriteSelection(const Vector2& position, const Vector2& size, const Vector2& pivot);

	std::shared_ptr<Texture> spriteToEdit;
	std::shared_ptr<Texture> oldSpriteToEdit; // TODO improve this by removing this and using events

	Vector2 offset = Vector2(0, 0);
	Vector2 minOffset = Vector2(-0.5f, -0.5f);
	Vector2 maxOffset = Vector2(0.5f, 0.5f);

	float zoom = 1;
	float minZoom = 0.2f;
	float maxZoom = 3;
	std::vector<SpriteSelection> spriteSelections;
	int currentSelectedSpriteIndex = -1;
};

