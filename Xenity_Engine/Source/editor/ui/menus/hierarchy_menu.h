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
	std::shared_ptr<GameObject> rightClickedElement;
	bool firstClickedInWindow = false;
};

