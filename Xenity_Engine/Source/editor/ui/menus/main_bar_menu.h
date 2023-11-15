#pragma once

#include "menu.h"
#include <memory>
class MainBarMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	template<typename T>
	void AddComponentToSelectedGameObject();
};
