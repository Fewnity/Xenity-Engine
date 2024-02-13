#pragma once

#include <memory>
#include "menu.h"
#include <engine/vectors/vector2.h>

class Texture;



class SpriteEditorMenu : public Menu
{
public:
	class SpriteSelection
	{
	public:
		Vector2 position;
		Vector2 size;
		Vector2 pivot;
	};
	
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
	void AddNewSpriteSelection(Vector2 position, Vector2 size, Vector2 pivot);
};

