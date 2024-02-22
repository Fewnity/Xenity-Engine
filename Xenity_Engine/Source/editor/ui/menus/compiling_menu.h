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
	~CompilingMenu();
	void Init() override;
	void Draw() override;
	void OpenPopup();
	void ClosePopup();

private:
	CompilingPupopState popupState = CompilingPupopState::Closed;
};

