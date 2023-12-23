#pragma once

#include <memory>
#include "menu.h"

class Texture;

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
	bool DrawImageButton(bool enabled, std::shared_ptr<Texture> texture);
	int height = 0;
};
