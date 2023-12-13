#pragma once

#include "menu.h"
#include <engine/vectors/vector2_int.h>
#include <memory>

class GameObject;

class SceneMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool isHovered = false;
	bool isFocused = false;
	Vector2Int windowSize = Vector2Int(0);
	Vector2Int windowPosition = Vector2Int(0);
	Vector2Int mousePosition = Vector2Int(0);
	bool startRotatingCamera = false;

private:
	void DrawToolWindow();
	void SetButtonColor(bool isSelected);
	std::weak_ptr<GameObject> cameraGO;
	bool isLastFrameOpened = false;
};

