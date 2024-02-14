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
	std::shared_ptr<Texture> spriteToEdit;

	Vector2 offset = Vector2(0, 0);
	Vector2 minOffset = Vector2(-0.5f, -0.5f);
	Vector2 maxOffset = Vector2(0.5f, 0.5f);

	float zoom = 1;
	float minZoom = 0.2f;
	float maxZoom = 3;
	std::vector<SpriteSelection> spriteSelections;
	int currentSelectedSpriteIndex = -1;

private:
	void AddNewSpriteSelection(const Vector2& position, const Vector2& size, const Vector2& pivot);
};

