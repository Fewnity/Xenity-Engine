#pragma once

#include "menu.h"
#include <memory>

class GameObject;

class HierarchyMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	std::weak_ptr<GameObject> rightClickedElement;
	bool firstClickedInWindow = false;
};

