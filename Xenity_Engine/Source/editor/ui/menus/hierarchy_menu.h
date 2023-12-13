#pragma once

#include "menu.h"
#include <memory>

class GameObject;

class HierarchyMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool isFocused = false;
private:
	std::shared_ptr<GameObject> rightClickedElement;
};

