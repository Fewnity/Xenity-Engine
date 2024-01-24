#pragma once

#include "menu.h"

enum class CompilingPupopState
{
	Closed,
	Closing,
	Opening
};

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

