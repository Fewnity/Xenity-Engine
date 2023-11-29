#pragma once

#include <memory>
#include "menu.h"

class MainBarMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	template<typename T>
	void AddComponentToSelectedGameObject();
};
