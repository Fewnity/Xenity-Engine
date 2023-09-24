#pragma once

#include "menu.h"

class CompilingMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void OpenPopup();
	void ClosePopup();
private:
	int popupState = 0;
};

