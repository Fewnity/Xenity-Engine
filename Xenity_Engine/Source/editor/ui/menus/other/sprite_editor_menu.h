// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <editor/ui/menus/menu.h>

#include <engine/math/vector2.h>
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

	std::shared_ptr<Texture> m_spriteToEdit;
	std::shared_ptr<Texture> m_oldSpriteToEdit; // TODO improve this by removing this and using events

	Vector2 m_offset = Vector2(0, 0);
	Vector2 m_minOffset = Vector2(-0.5f, -0.5f);
	Vector2 m_maxOffset = Vector2(0.5f, 0.5f);

	float m_zoom = 1;
	float m_minZoom = 0.2f;
	float m_maxZoom = 3;
	std::vector<SpriteSelection> m_spriteSelections;
	int m_currentSelectedSpriteIndex = -1;
};

