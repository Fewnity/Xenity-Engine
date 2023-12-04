#pragma once

#include <memory>
#include "menu.h"

class MainBarMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	int GetHeight()
	{
		return height;
	}

private:
	template<typename T>
	void AddComponentToSelectedGameObject();
	int height = 0;
};
