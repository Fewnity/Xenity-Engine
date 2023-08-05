#pragma once

#include "menu.h"

class CompilingMenu : public Menu
{
public:
	void Init();
	void Draw();
	void OpenPopup();
	void ClosePopup();
private:
	int popupState = 0;
};

